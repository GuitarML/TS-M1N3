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
TS-M1N3AudioProcessor::TS-M1N3AudioProcessor()
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

                        std::make_unique<AudioParameterFloat>(MASTER_ID, MASTER_NAME, NormalisableRange<float>(-36.0f, 0.0f, 0.01f), 0.0f) })

#endif
{
    loadConfig(jsonFiles[current_model_index]);

}


TS-M1N3AudioProcessor::~TS-M1N3AudioProcessor()
{
}

//==============================================================================
const String TS-M1N3AudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool TS-M1N3AudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool TS-M1N3AudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool TS-M1N3AudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double TS-M1N3AudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int TS-M1N3AudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int TS-M1N3AudioProcessor::getCurrentProgram()
{
    return 0;
}

void TS-M1N3AudioProcessor::setCurrentProgram (int index)
{
}

const String TS-M1N3AudioProcessor::getProgramName (int index)
{
    return {};
}

void TS-M1N3AudioProcessor::changeProgramName (int index, const String& newName)
{
}

//==============================================================================
void TS-M1N3AudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    LSTM.reset();

    // set up DC blocker
    dcBlocker.coefficients = dsp::IIR::Coefficients<float>::makeHighPass(sampleRate, 35.0f);
    dsp::ProcessSpec spec{ sampleRate, static_cast<uint32> (samplesPerBlock), 2 };
    dcBlocker.prepare(spec);
}

void TS-M1N3AudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool TS-M1N3AudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
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


void TS-M1N3AudioProcessor::processBlock (AudioBuffer<float>& buffer, MidiBuffer& midiMessages)
{
    ScopedNoDenormals noDenormals;

    // Setup Audio Data
    const int numSamples = buffer.getNumSamples();
    const int numInputChannels = getTotalNumInputChannels();
    const int sampleRate = getSampleRate();

    auto block = dsp::AudioBlock<float>(buffer).getSingleChannelBlock(0);
    auto context = juce::dsp::ProcessContextReplacing<float>(block);

    // Amp =============================================================================
    if (amp_state == 1) {


        //buffer.applyGain(gain * 2.0);
 

   
        // Apply LSTM model

        LSTM.process(buffer.getReadPointer(0), driveValue, toneValue, buffer.getWritePointer(0), numSamples);
      
        //    Master Volume 
        buffer.applyGain(masterValue * 2.0); // Adding volume range (2x) mainly for clean models

    }

    // process DC blocker
    auto monoBlock = dsp::AudioBlock<float>(buffer).getSingleChannelBlock(0);
    dcBlocker.process(dsp::ProcessContextReplacing<float>(monoBlock));
    
    for (int ch = 1; ch < buffer.getNumChannels(); ++ch)
        buffer.copyFrom(ch, 0, buffer, 0, 0, buffer.getNumSamples());
}

//==============================================================================
bool TS-M1N3AudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* TS-M1N3AudioProcessor::createEditor()
{
    return new TS-M1N3AudioProcessorEditor (*this);
}

//==============================================================================
void TS-M1N3AudioProcessor::getStateInformation (MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.

    auto state = treeState.copyState();
    std::unique_ptr<XmlElement> xml (state.createXml());
    xml->setAttribute ("current_tone", current_model_index);
    copyXmlToBinary (*xml, destData);
}

void TS-M1N3AudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.

    std::unique_ptr<juce::XmlElement> xmlState (getXmlFromBinary (data, sizeInBytes));

    if (xmlState.get() != nullptr)
    {
        if (xmlState->hasTagName (treeState.state.getType()))
        {
            treeState.replaceState (juce::ValueTree::fromXml (*xmlState));
            current_model_index = xmlState->getIntAttribute ("current_tone");

            switch (current_model_index)
            {
            case 0:
                loadConfig (red_tone);
                break;
            case 1:
                loadConfig (gold_tone);
                break;
            case 2:
                loadConfig (green_tone);
                break;
            }

            if (auto* editor = dynamic_cast<TS-M1N3AudioProcessorEditor*> (getActiveEditor()))
                editor->resetImages();
        }
    }
}



void TS-M1N3AudioProcessor::loadConfig(File configFile)
{
    this->suspendProcessing(true);
    String path = configFile.getFullPathName();
    char_filename = path.toUTF8();

    try {
        // Check input size for conditioned models
        // read JSON file
        std::ifstream i2(char_filename);
        nlohmann::json weights_json;
        i2 >> weights_json;

        int input_size_json = weights_json["/model_data/input_size"_json_pointer];
        LSTM.input_size = input_size_json;
        if (input_size_json == 1) {
            is_conditioned = false;
            LSTM.load_json(char_filename);
        }
        else {
            is_conditioned = true;
            LSTM.load_json2(char_filename);
        }
        model_loaded = 1;
    }
    catch (const std::exception& e) {
        DBG("Unable to load json file: " + configFile.getFullPathName());
        std::cout << e.what();
    }

    this->suspendProcessing(false);
}


float TS-M1N3AudioProcessor::convertLogScale(float in_value, float x_min, float x_max, float y_min, float y_max)
{
    float b = log(y_max / y_min) / (x_max - x_min);
    float a = y_max / exp(b * x_max);
    float converted_value = a * exp(b * in_value);
    return converted_value;
}


float TS-M1N3AudioProcessor::decibelToLinear(float dbValue)
{
    return powf(10.0, dbValue/20.0);
}

void TS-M1N3AudioProcessor::setDrive(float paramDrive)
{
    driveValue = paramDrive;
}

void TS-M1N3AudioProcessor::setTone(float paramTone)
{
    toneValue = paramTone;
}

void TS-M1N3AudioProcessor::setMaster(float db_ampMaster)
{
    ampMasterKnobState = db_ampMaster;
    if (db_ampMaster == -36.0) {
        ampMaster = decibelToLinear(-100.0);
    } else {
        ampMaster = decibelToLinear(db_ampMaster);
    }
}


//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new TS-M1N3AudioProcessor();
}
