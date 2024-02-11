/*
  ==============================================================================

    ControlBar.h
    Created: 2 Sep 2023 7:39:03pm
    Author:  Paxton Fleming

  ==============================================================================
*/

#pragma once
#include "JuceHeader.h"
#include "PluginProcessor.h"
#include "LookAndFeel.h"
#include "Params.h"

class ControlBar : public juce::Component
{
public:
    
    ControlBar(PaxMBClipAudioProcessor& inProcessor);

    void paint(juce::Graphics& g) override;
    void resized() override;

    AnalyzerButton analyzerButton;
    PowerButton globalBypassButton;
private:
    PaxMBClipAudioProcessor* m_processor;
    juce::TextButton masterClipButton;
};