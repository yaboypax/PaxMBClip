/*
  ==============================================================================

    ClipperBandControls.h
    Created: 29 Aug 2023 8:06:37pm
    Author:  flemi

  ==============================================================================
*/

#pragma once
#include "JuceHeader.h"
#include "Utilities.h"
#include "RotarySlider.h"
#include "LookAndFeel.h"
#include "Params.h"

struct ClipperBandControls : juce::Component
{
    ClipperBandControls(juce::AudioProcessorValueTreeState& apvts);
    void paint(juce::Graphics& g) override;
    void resized() override;
    void updateAttachments();


private:
    juce::AudioProcessorValueTreeState& apvts;

    using Attachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    RotarySlider bandGainSlider, bandClipSlider;
    std::unique_ptr<Attachment> bandGainSliderAttachment, bandClipSliderAttachment;

    using BtnAttachment = juce::AudioProcessorValueTreeState::ButtonAttachment;
    juce::ToggleButton bypassButton, soloButton, muteButton;
    std::unique_ptr<BtnAttachment> bypassButtonAttachment, soloButtonAttachment, muteButtonAttachment;

    juce::ToggleButton lowBandButton, midBandButton, highBandButton;

    juce::Component::SafePointer<ClipperBandControls> safePtr{ this };
};