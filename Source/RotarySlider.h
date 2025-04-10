/*
  ==============================================================================

    RotarySlider.h
    Created: 29 Aug 2023 8:01:17pm
    Author:  flemi

  ==============================================================================
*/

#pragma once
#include "JuceHeader.h"

struct RotarySlider : juce::Slider
{
    RotarySlider() : juce::Slider(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag,
        juce::Slider::TextEntryBoxPosition::NoTextBox){}

    int getTextHeight() const { return 14; }
    void changeParam(juce::RangedAudioParameter* p)
    {
        param = p;
        repaint();
    }
private:
    juce::RangedAudioParameter* param = nullptr;
    juce::String suffix;
};

