/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "ClipperBandControls.h"
#include "GlobalControls.h"
#include "SpectrumAnalyzer.h"
//==============================================================================

struct ControlBar : juce::Component
{
    ControlBar();
    void resized() override;

    AnalyzerButton analyzerButton;
    PowerButton globalBypassButton;

    LookAndFeel lnf;
};

struct Placeholder : juce::Component
{
    Placeholder();

    void paint(juce::Graphics& g) override
    {
        g.fillAll(customColor);
    }
    juce::Colour customColor;
};




/**
*/

class PaxMBClipAudioProcessorEditor  : public juce::AudioProcessorEditor, public juce::Timer
{
public:
    PaxMBClipAudioProcessorEditor (PaxMBClipAudioProcessor& p);
    ~PaxMBClipAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

    void timerCallback();

private:
    PaxMBClipAudioProcessor& audioProcessor;

    ControlBar controlBar;
    GlobalControls globalControls {audioProcessor.apvts};
    ClipperBandControls bandControls {audioProcessor.apvts};
    SpectrumAnalyzer analyzer {audioProcessor};

    void toggleGlobalBypassState();
    void updateGlobalBypassButton();

    std::array<juce::AudioParameterBool*, 3> getBypassParams();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PaxMBClipAudioProcessorEditor)
};
