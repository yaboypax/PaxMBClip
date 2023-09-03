/*
  ==============================================================================

    GlobalControls.h
    Created: 29 Aug 2023 8:20:08pm
    Author:  flemi

  ==============================================================================
*/

#pragma once

#include "JuceHeader.h"
#include "Utilities.h"
#include "RotarySlider.h"
#include "Params.h"


struct GlobalControls : juce::Component
{
    GlobalControls(juce::AudioProcessorValueTreeState& apvts);
    void paint(juce::Graphics& g) override;
    void resized() override;

private:
    RotarySlider inGainSlider, lowMidXoverSlider, midHighXoverSlider, outGainSlider;

    using Attachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    std::unique_ptr<Attachment> lowMidXoverSliderAttachment, midHighXoverSliderAttachment, inGainSliderAttachment, outGainSliderAttachment;

};
