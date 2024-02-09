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
#include "ControlBar.h"
#include "LookAndFeel.h"
//===============================================================================

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
    LookAndFeel lnf;

    
    ControlBar controlBar;
    GlobalControls globalControls {audioProcessor.apvts};
    ClipperBandControls bandControls {audioProcessor};
    SpectrumAnalyzer analyzer {audioProcessor};

    void toggleGlobalBypassState();
    void updateGlobalBypassButton();

    std::array<juce::AudioParameterBool*, 3> getBypassParams();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PaxMBClipAudioProcessorEditor)
};
