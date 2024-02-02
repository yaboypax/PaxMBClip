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
    RotarySlider lowMidXoverSlider, midHighXoverSlider;
    juce::Slider inGainSlider, outGainSlider;

    using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    std::unique_ptr<SliderAttachment> lowMidXoverSliderAttachment, midHighXoverSliderAttachment, inGainSliderAttachment, outGainSliderAttachment;

    juce::ComboBox oversamplingSelection;
    
    using ComboBoxAttachment = juce::AudioProcessorValueTreeState::ComboBoxAttachment;
    std::unique_ptr<ComboBoxAttachment> oversamplingAttachment;

};
