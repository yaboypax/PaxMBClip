/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================

struct Placeholder : juce::Component
{
    Placeholder();

    void paint(juce::Graphics& g) override
    {
        g.fillAll(customColor);
    }
    juce::Colour customColor;
};

struct GlobalControls : juce::Component
{
    void paint(juce::Graphics& g) override;
};


/**
*/
class PaxMBClipAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    PaxMBClipAudioProcessorEditor (PaxMBClipAudioProcessor&);
    ~PaxMBClipAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    PaxMBClipAudioProcessor& audioProcessor;

    Placeholder controlBar, analyzer, bandControls;
    GlobalControls globalControls;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PaxMBClipAudioProcessorEditor)
};
