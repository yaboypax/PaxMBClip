/*
  ==============================================================================

    RotarySlider.h
    Created: 29 Aug 2023 8:01:17pm
    Author:  flemi

  ==============================================================================
*/

#pragma once
#include "JuceHeader.h"

struct RotarySlider : juce::Slider , juce::MouseListener
{
    RotarySlider() : juce::Slider(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag,
        juce::Slider::TextEntryBoxPosition::NoTextBox)
    {
        param = nullptr;
    }

    int getTextHeight() const { return 14; }
    void changeParam(juce::RangedAudioParameter* p);

    void mouseEnter(const juce::MouseEvent& ev) override
    {
        getParentComponent()->mouseEnter(ev);
    }

    void mouseExit(const juce::MouseEvent& ev) override
    {
        getParentComponent()->mouseExit(ev);
    }

private:
    juce::RangedAudioParameter* param;
    juce::String suffix;
};










//
//struct RotarySliderWithLabels : juce::Slider
//{
//    RotarySliderWithLabels(juce::RangedAudioParameter& rap, const juce::String& unitSuffix) :
//        juce::Slider(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag,
//            juce::Slider::TextEntryBoxPosition::NoTextBox),
//        param(&rap),
//        suffix(unitSuffix)
//    {
//        setLookAndFeel(&lnf);
//    }
//
//    ~RotarySliderWithLabels()
//    {
//        setLookAndFeel(nullptr);
//    }
//
//    struct LabelPos
//    {
//        float pos;
//        juce::String label;
//    };
//
//    juce::Array<LabelPos> labels;
//
//    void paint(juce::Graphics& g) override;
//    juce::Rectangle<int> getSliderBounds() const;
//    int getTextHeight() const { return 14; }
//    juce::String getDisplayString() const;
//private:
//    LookAndFeel lnf;
//
//    juce::RangedAudioParameter* param;
//    juce::String suffix;
//};

