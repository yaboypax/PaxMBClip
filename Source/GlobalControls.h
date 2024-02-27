/*
  ==============================================================================

    GlobalControls.h
    Created: 29 Aug 2023 8:20:08pm
    Author:  flemi

  ==============================================================================
*/

#pragma once

#include "JuceHeader.h"
#include "PluginProcessor.h"
#include "Utilities.h"
#include "LookAndFeel.h"
#include "Params.h"


struct GlobalControls : juce::Component
{
    GlobalControls(PaxMBClipAudioProcessor& inProcessor);
    ~GlobalControls();
    void paint(juce::Graphics& g) override;
    void resized() override;

    void setupLevelMeters();

private:
    PaxMBClipAudioProcessor* m_processor;
    juce::Slider inGainSlider, outGainSlider;

    using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    std::unique_ptr<SliderAttachment> inGainSliderAttachment, outGainSliderAttachment;

    LevelMeterLookAndFeel* levelMeterInLAF{ new LevelMeterLookAndFeelVertical() };
    LevelMeterLookAndFeel* levelMeterOutLAF{ new LevelMeterLookAndFeelVertical() };

    LevelMeter m_inputMeter{ levelMeterInLAF };
    LevelMeter m_outputMeter{ levelMeterOutLAF };


    juce::TextButton masterClipButton;

};
