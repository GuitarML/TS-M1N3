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

#define GAIN_ID "gain"
#define GAIN_NAME "Gain"

#define MASTER_ID "master"
#define MASTER_NAME "Master"

#define TONE_ID "tone"
#define TONE_NAME "Tone"


//==============================================================================
/**
*/
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
    AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

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
    
    float convertLogScale(float in_value, float x_min, float x_max, float y_min, float y_max);

    float decibelToLinear(float dbValue);

    void setDrive(float paramDrive);
    void setTone(float paramTone);
    void setMaster(float db_ampMaster);

    void loadConfig();

    // Pedal/amp states
    int fw_state = 1; // 0 = off, 1 = on
    float driveValue;
    float toneValue;
    float masterValue;

    File loaded_tone;
    juce::String loaded_tone_name;
    const char* char_filename = "";

    bool lstm_state = true;

    juce::String loaded_ir_name;

    RT_LSTM LSTM;

    AudioProcessorValueTreeState treeState;

    const char* jsonFile = "C:\\Users\\rache\\Desktop\\dev\\TS-M1N3\\models\\model_ts9_cond2.json";

private:
    var dummyVar;


    dsp::IIR::Filter<float> dcBlocker;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TSM1N3AudioProcessor)
};
