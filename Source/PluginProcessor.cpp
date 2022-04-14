/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"

#include <iostream>
#include <fstream>

//==============================================================================
TSM1N3AudioProcessor::TSM1N3AudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
    : AudioProcessor(BusesProperties()
#if ! JucePlugin_IsMidiEffect
#if ! JucePlugin_IsSynth
        .withInput("Input", AudioChannelSet::mono(), true)
#endif
        .withOutput("Output", AudioChannelSet::mono(), true)
#endif
    )  
    //treeState(*this, nullptr, "PARAMETER", { std::make_unique<AudioParameterFloat>(GAIN_ID, GAIN_NAME, NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.5f),
    //                    std::make_unique<AudioParameterFloat>(TONE_ID, TONE_NAME, NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.5f),
    //                    std::make_unique<AudioParameterFloat>(MASTER_ID, MASTER_NAME, NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.5) })

#endif
{
    // initialize parameters:
    addParameter(gainParam = new AudioParameterFloat(GAIN_ID, GAIN_NAME, NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.5f));
    addParameter(toneParam = new AudioParameterFloat(TONE_ID, TONE_NAME, NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.5f));
    addParameter(masterParam = new AudioParameterFloat(MASTER_ID, MASTER_NAME, NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.5f));
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
    if (layouts.getMainOutputChannelSet() != AudioChannelSet::mono())
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

    // Amp =============================================================================
    if (fw_state == 1) {
        auto gain = static_cast<float> (gainParam->get());
        auto tone = static_cast<float> (toneParam->get());
        auto master = static_cast<float> (masterParam->get());

        auto block = dsp::AudioBlock<float>(buffer.getArrayOfWritePointers(), 1, numSamples);

        // Apply LSTM model
        LSTM.process(block.getChannelPointer(0), gain, tone, block.getChannelPointer(0), (int) block.getNumSamples());

        // Master Volume 
        // Apply ramped changes for gain smoothing
        if (master == previousMasterValue)
        {
            buffer.applyGain(master);
        }
        else {
            buffer.applyGainRamp(0, (int) buffer.getNumSamples(), previousMasterValue , master);
            previousMasterValue = master;
        }
    }

}

//==============================================================================

#if !JUCE_AUDIOPROCESSOR_NO_GUI
bool TSM1N3AudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}


AudioProcessorEditor* TSM1N3AudioProcessor::createEditor()
{
    return new TSM1N3AudioProcessorEditor (*this);
}
#endif

//==============================================================================
void TSM1N3AudioProcessor::getStateInformation (MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.

    MemoryOutputStream stream(destData, true);

    stream.writeFloat(*gainParam);
    stream.writeFloat(*masterParam);
    stream.writeFloat(*toneParam);
}

void TSM1N3AudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.

    MemoryInputStream stream(data, static_cast<size_t> (sizeInBytes), false);

    gainParam->setValueNotifyingHost(stream.readFloat());
    masterParam->setValueNotifyingHost(stream.readFloat());
    toneParam->setValueNotifyingHost(stream.readFloat());
}



//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new TSM1N3AudioProcessor();
}
