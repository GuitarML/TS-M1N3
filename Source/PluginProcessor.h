/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include <nlohmann/json.hpp>
#include "RTNeuralLSTM.h"

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

#define GAIN_ID "drive"
#define GAIN_NAME "Drive"
#define MASTER_ID "level"
#define MASTER_NAME "Level"
#define TONE_ID "tone"
#define TONE_NAME "Tone"


//==============================================================================
class TSM1N3AudioProcessor  : public AudioProcessor
{
public:
    //==============================================================================
    TSM1N3AudioProcessor();
    ~TSM1N3AudioProcessor();

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (AudioBuffer<float>&, MidiBuffer&) override;

    //==============================================================================
    #if !JUCE_AUDIOPROCESSOR_NO_GUI
    AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;
    #endif
    //==============================================================================
    const String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const String getProgramName (int index) override;
    void changeProgramName (int index, const String& newName) override;

    //==============================================================================
    void getStateInformation (MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;
    
    //float convertLogScale(float in_value, float x_min, float x_max, float y_min, float y_max);

    // Pedal/amp states
    int fw_state = 1; // 0 = off, 1 = on
    //float driveValue = 0.5;
    //float toneValue = 0.5;
    //float masterValue = 0.5;
    float previousMasterValue = 0.5;

    bool lstm_state = true;;

    RT_LSTM LSTM;

    //AudioProcessorValueTreeState treeState;

private:

    AudioParameterFloat* gainParam;
    AudioParameterFloat* toneParam;
    AudioParameterFloat* masterParam;

    //dsp::IIR::Filter<float> dcBlocker;  // Unused for TS-M1N3 plugin, leaving commented as template for future plugins

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TSM1N3AudioProcessor)
};
