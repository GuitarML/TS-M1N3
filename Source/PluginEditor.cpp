/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <stdio.h>
#include <fstream>
#include <iostream>


//==============================================================================
TS-M1N3AudioProcessorEditor::TS-M1N3AudioProcessorEditor (TS-M1N3AudioProcessor& p)
    : AudioProcessorEditor (&p), processor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to

    // Set Widget Graphics
    ampSilverKnobLAF.setLookAndFeel(ImageCache::getFromMemory(BinaryData::knob_70_black_png, BinaryData::knob_70_black_pngSize));

    addAndMakeVisible(colorSelectButton);
    colorSelectButton.setImages(true, true, true,
        ImageCache::getFromMemory(BinaryData::power_switch_up_png, BinaryData::power_switch_up_pngSize), 1.0, Colours::transparentWhite,
        Image(), 1.0, Colours::transparentWhite,
        ImageCache::getFromMemory(BinaryData::power_switch_up_png, BinaryData::power_switch_up_pngSize), 1.0, Colours::transparentWhite,
        0.0);
    colorSelectButton.addListener(this);
    

    addAndMakeVisible(ampLED);
    ampLED.setImages(true, true, true,
        ImageCache::getFromMemory(BinaryData::led_red_on_png, BinaryData::led_red_on_pngSize), 1.0, Colours::transparentWhite,
        Image(), 1.0, Colours::transparentWhite,
        ImageCache::getFromMemory(BinaryData::led_red_on_png, BinaryData::led_red_on_pngSize), 1.0, Colours::transparentWhite,
        0.0);
    ampLED.addListener(this);



    gainSliderAttach = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(processor.treeState, GAIN_ID, ampGainKnob);
    addAndMakeVisible(ampGainKnob);
    ampGainKnob.setLookAndFeel(&ampSilverKnobLAF);
    ampGainKnob.addListener(this);
    ampGainKnob.setRange(-10.0, 10.0);
    ampGainKnob.setValue(processor.ampGainKnobState);
    ampGainKnob.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    ampGainKnob.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::NoTextBox, false, 50, 20);
    ampGainKnob.setNumDecimalPlacesToDisplay(1);
    ampGainKnob.setDoubleClickReturnValue(true, 0.0);
	
    toneSliderAttach = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(processor.treeState, TONE_ID, ampToneKnob);
    addAndMakeVisible(ampPresenceKnob);
    ampToneKnob.setLookAndFeel(&ampSilverKnobLAF);
    ampToneKnob.addListener(this);
    ampToneKnob.setRange(-8.0, 8.0);
    ampToneKnob.setValue(processor.ampPresenceKnobState);
    ampToneKnob.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    ampToneKnob.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::NoTextBox, false, 50, 20 );
    ampToneKnob.setNumDecimalPlacesToDisplay(1);
    ampToneKnob.setDoubleClickReturnValue(true, 0.0);

    masterSliderAttach = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(processor.treeState, MASTER_ID, ampMasterKnob);
    addAndMakeVisible(ampMasterKnob);
    ampMasterKnob.setLookAndFeel(&ampSilverKnobLAF);
    ampMasterKnob.addListener(this);
    ampMasterKnob.setRange(-36.0, 0.0);
    ampMasterKnob.setValue(processor.ampMasterKnobState);
    ampMasterKnob.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    ampMasterKnob.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::NoTextBox, false, 50, 20 );
    ampMasterKnob.setNumDecimalPlacesToDisplay(1);
    ampMasterKnob.setDoubleClickReturnValue(true, -18.0);

    // Size of plugin GUI
    setSize(370, 560);
}

TS-M1N3AudioProcessorEditor::~TS-M1N3AudioProcessorEditor()
{
    ampBassKnob.setLookAndFeel(nullptr);
    ampToneKnob.setLookAndFeel(nullptr);
    ampMasterKnob.setLookAndFeel(nullptr);
}

//==============================================================================
void TS-M1N3AudioProcessorEditor::paint (Graphics& g)
{
    // Workaround for graphics on Windows builds (clipping code doesn't work correctly on Windows)
    #if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
    g.drawImageAt(background, 0, 0);  // Debug Line: Redraw entire background image
    #else
    // Redraw only the clipped part of the background image
    juce::Rectangle<int> ClipRect = g.getClipBounds(); 
    g.drawImage(background, ClipRect.getX(), ClipRect.getY(), ClipRect.getWidth(), ClipRect.getHeight(), ClipRect.getX(), ClipRect.getY(), ClipRect.getWidth(), ClipRect.getHeight());
    #endif
   
}

void TS-M1N3AudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..

    // Amp Widgets
    ampGainKnob.setBounds(10, 120, 75, 95);
    ampMasterKnob.setBounds(95, 120, 75, 95);
    ampToneKnob.setBounds(10, 250, 75, 95);
}



void SmartPedalAudioProcessorEditor::buttonClicked(juce::Button* button)
{
    odFootSwClicked();
}

void SmartPedalAudioProcessorEditor::odFootSwClicked() {
    if (processor.od_state == 0)
        processor.od_state = 1;
    else
        processor.od_state = 0;
    repaint();
}


void TS-M1N3AudioProcessorEditor::sliderValueChanged(Slider* slider)
{
    // Amp
    if (slider == &ampGainKnob)
        processor.setDrive(slider->getValue());
    else if (slider == &ampMasterKnob)
        processor.setMaster(slider->getValue());
    else if (slider == &ampToneKnob) 
        processor.setTone(slider->getValue());
}

void TS-M1N3AudioProcessorEditor::resetImages()
{
    if (processor.current_model_index == 0) {
        colorSelectButton.setImages(true, true, true,
            ImageCache::getFromMemory(BinaryData::power_switch_up_png, BinaryData::power_switch_up_pngSize), 1.0, Colours::transparentWhite,
            Image(), 1.0, Colours::transparentWhite,
            ImageCache::getFromMemory(BinaryData::power_switch_up_png, BinaryData::power_switch_up_pngSize), 1.0, Colours::transparentWhite,
            0.0);
        ampLED.setImages(true, true, true,
            ImageCache::getFromMemory(BinaryData::led_red_on_png, BinaryData::led_red_on_pngSize), 1.0, Colours::transparentWhite,
            Image(), 1.0, Colours::transparentWhite,
            ImageCache::getFromMemory(BinaryData::led_red_on_png, BinaryData::led_red_on_pngSize), 1.0, Colours::transparentWhite,
            0.0);
    }
    else if (processor.current_model_index == 1) {
        colorSelectButton.setImages(true, true, true,
            ImageCache::getFromMemory(BinaryData::power_switch_mid_png, BinaryData::power_switch_mid_pngSize), 1.0, Colours::transparentWhite,
            Image(), 1.0, Colours::transparentWhite,
            ImageCache::getFromMemory(BinaryData::power_switch_mid_png, BinaryData::power_switch_mid_pngSize), 1.0, Colours::transparentWhite,
            0.0);
        ampLED.setImages(true, true, true,
            ImageCache::getFromMemory(BinaryData::led_gold_on_png, BinaryData::led_gold_on_pngSize), 1.0, Colours::transparentWhite,
            Image(), 1.0, Colours::transparentWhite,
            ImageCache::getFromMemory(BinaryData::led_gold_on_png, BinaryData::led_gold_on_pngSize), 1.0, Colours::transparentWhite,
            0.0);
    }
    else {
        colorSelectButton.setImages(true, true, true,
            ImageCache::getFromMemory(BinaryData::power_switch_down_png, BinaryData::power_switch_down_pngSize), 1.0, Colours::transparentWhite,
            Image(), 1.0, Colours::transparentWhite,
            ImageCache::getFromMemory(BinaryData::power_switch_down_png, BinaryData::power_switch_down_pngSize), 1.0, Colours::transparentWhite,
            0.0);
        ampLED.setImages(true, true, true,
            ImageCache::getFromMemory(BinaryData::led_green_on_png, BinaryData::led_green_on_pngSize), 1.0, Colours::transparentWhite,
            Image(), 1.0, Colours::transparentWhite,
            ImageCache::getFromMemory(BinaryData::led_green_on_png, BinaryData::led_green_on_pngSize), 1.0, Colours::transparentWhite,
            0.0);
    }
}