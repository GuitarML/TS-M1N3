/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <iostream>
#include <fstream>

//==============================================================================
TSM1N3AudioProcessor::TSM1N3AudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
    : AudioProcessor(BusesProperties()
#if ! JucePlugin_IsMidiEffect
#if ! JucePlugin_IsSynth
        .withInput("Input", AudioChannelSet::stereo(), true)
#endif
        .withOutput("Output", AudioChannelSet::stereo(), true)
#endif
    ),    
    treeState(*this, nullptr, "PARAMETER", { std::make_unique<AudioParameterFloat>(GAIN_ID, GAIN_NAME, NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.0f),
                        std::make_unique<AudioParameterFloat>(TONE_ID, TONE_NAME, NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.0f),
                        std::make_unique<AudioParameterFloat>(MASTER_ID, MASTER_NAME, NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.5) })

#endif
{
}


TSM1N3AudioProcessor::~TSM1N3AudioProcessor()
{
}

//==============================================================================
const String TSM1N3AudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool TSM1N3AudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool TSM1N3AudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool TSM1N3AudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double TSM1N3AudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int TSM1N3AudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int TSM1N3AudioProcessor::getCurrentProgram()
{
    return 0;
}

void TSM1N3AudioProcessor::setCurrentProgram (int index)
{
}

const String TSM1N3AudioProcessor::getProgramName (int index)
{
    return {};
}

void TSM1N3AudioProcessor::changeProgramName (int index, const String& newName)
{
}

//==============================================================================
void TSM1N3AudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    LSTM.reset();

    // prepare resampler for target sample rate: 48 kHz
    constexpr double targetSampleRate = 48000.0;
    resampler.prepareWithTargetSampleRate ({ sampleRate, (uint32) samplesPerBlock, 1 }, targetSampleRate);

    // load 48 kHz sample rate model
    MemoryInputStream jsonInputStream(BinaryData::model_ts9_48k_cond2_json, BinaryData::model_ts9_48k_cond2_jsonSize, false);
    nlohmann::json weights_json = nlohmann::json::parse(jsonInputStream.readEntireStreamAsString().toStdString());

    LSTM.load_json3(weights_json);

    // set up DC blocker
    //dcBlocker.coefficients = dsp::IIR::Coefficients<float>::makeHighPass(sampleRate, 35.0f);
    //dsp::ProcessSpec spec{ sampleRate, static_cast<uint32> (samplesPerBlock), 2 };
    //dcBlocker.prepare(spec);
}

void TSM1N3AudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool TSM1N3AudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    if (layouts.getMainOutputChannelSet() != AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif


void TSM1N3AudioProcessor::processBlock (AudioBuffer<float>& buffer, MidiBuffer& midiMessages)
{
    ScopedNoDenormals noDenormals;

    // Setup Audio Data
    const int numSamples = buffer.getNumSamples();

    // resample to target sample rate
    auto block = dsp::AudioBlock<float> (buffer.getArrayOfWritePointers(), 1, numSamples);
    auto block48k = resampler.processIn (block);

    // Amp =============================================================================
    if (fw_state == 1) {
        // Apply LSTM model
        LSTM.process(block48k.getChannelPointer(0), driveValue, toneValue, block48k.getChannelPointer(0), (int) block48k.getNumSamples());
      
        // Master Volume 
        // Apply ramped changes for gain smoothing
        if (masterValue == previousMasterValue)
        {
            buffer.applyGain(masterValue);
        }
        else {
            buffer.applyGainRamp(0, (int) block48k.getNumSamples(), previousMasterValue , masterValue);
            previousMasterValue = masterValue;
        }
    }

    // resample back to original sample rate
    resampler.processOut (block48k, block);

    // process DC blocker
    //auto monoBlock = dsp::AudioBlock<float>(buffer).getSingleChannelBlock(0);
    //dcBlocker.process(dsp::ProcessContextReplacing<float>(monoBlock));
    
    for (int ch = 1; ch < buffer.getNumChannels(); ++ch)
        buffer.copyFrom(ch, 0, buffer, 0, 0, buffer.getNumSamples());
}

//==============================================================================
bool TSM1N3AudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* TSM1N3AudioProcessor::createEditor()
{
    return new TSM1N3AudioProcessorEditor (*this);
}

//==============================================================================
void TSM1N3AudioProcessor::getStateInformation (MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.

    auto state = treeState.copyState();
    std::unique_ptr<XmlElement> xml (state.createXml());
    copyXmlToBinary (*xml, destData);
}

void TSM1N3AudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.

    std::unique_ptr<juce::XmlElement> xmlState (getXmlFromBinary (data, sizeInBytes));

    if (xmlState.get() != nullptr)
    {
        if (xmlState->hasTagName (treeState.state.getType()))
        {
            treeState.replaceState (juce::ValueTree::fromXml (*xmlState));

            if (auto* editor = dynamic_cast<TSM1N3AudioProcessorEditor*> (getActiveEditor()))
                editor->resetImages();
        }
    }
}

/* // UNUSED but kept as a template for future plugins
float TSM1N3AudioProcessor::convertLogScale(float in_value, float x_min, float x_max, float y_min, float y_max)
{
    float b = log(y_max / y_min) / (x_max - x_min);
    float a = y_max / exp(b * x_max);
    float converted_value = a * exp(b * in_value);
    return converted_value;
}
*/

float TSM1N3AudioProcessor::decibelToLinear(float dbValue)
{
    return powf(10.0, dbValue/20.0);
}

void TSM1N3AudioProcessor::setDrive(float paramDrive)
{
    driveValue = paramDrive;
}

void TSM1N3AudioProcessor::setTone(float paramTone)
{
    toneValue = paramTone;
}

void TSM1N3AudioProcessor::setMaster(float db_ampMaster)
{
    masterValue = db_ampMaster;
}


//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new TSM1N3AudioProcessor();
}
