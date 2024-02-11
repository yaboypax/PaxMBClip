/*
  ==============================================================================

    GlobalControls.cpp
    Created: 29 Aug 2023 8:20:08pm
    Author:  flemi

  ==============================================================================
*/

#include "GlobalControls.h"

namespace
{
    constexpr int sliderX = 26;
    constexpr int sliderY = 53;
    constexpr int sliderWidth = 48;
    constexpr int sliderHeight = 355;

    constexpr int overX = 26;
    constexpr int overY = 22;
    constexpr int overW = 100;
    constexpr int overH = 20;

    constexpr int xoverX = 16;
    constexpr int xoverY = 405;
    constexpr int xOverSize = 58;
}

static juce::String getOversamplingPower(int ovr)
{
    return static_cast<juce::String>(pow(2, ovr)) + "x Oversampling";
}

GlobalControls::GlobalControls(juce::AudioProcessorValueTreeState& apvts)
{
    using namespace Params;
    const auto& params = GetParams();

    makeAttachment(inGainSliderAttachment, apvts, params, Names::Gain_In, inGainSlider);
    makeAttachment(lowMidXoverSliderAttachment, apvts, params, Names::Low_Mid_Crossover_Freq, lowMidXoverSlider);
    makeAttachment(midHighXoverSliderAttachment, apvts, params, Names::Mid_High_Crossover_Freq, midHighXoverSlider);
    makeAttachment(outGainSliderAttachment, apvts, params, Names::Gain_Out, outGainSlider);

    inGainSlider.setSliderStyle(juce::Slider::LinearVertical);
    inGainSlider.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
    inGainSlider.setRange(-24.0, 24.0, 0.1);

    outGainSlider.setSliderStyle(juce::Slider::LinearVertical);
    outGainSlider.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
    outGainSlider.setRange(-24.0, 24.0, 0.1);

    addAndMakeVisible(inGainSlider);
    addAndMakeVisible(lowMidXoverSlider);
    addAndMakeVisible(midHighXoverSlider);
    addAndMakeVisible(outGainSlider);

    for (int i = 0; i <= 5; ++i)
    {
        oversamplingSelection.addItem(getOversamplingPower(i), i + 1);
    }

    makeAttachment(oversamplingAttachment, apvts, params, Names::Oversample, oversamplingSelection);
    addAndMakeVisible(oversamplingSelection);
}

void GlobalControls::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds();
    drawModuleBackground(g, bounds);
}

void GlobalControls::resized()
{
    inGainSlider.setBounds(sliderX, sliderY, sliderWidth, sliderHeight);
    outGainSlider.setBounds(inGainSlider.getRight() + 5, sliderY, sliderWidth, sliderHeight);

    lowMidXoverSlider.setBounds(xoverX, xoverY, xOverSize, xOverSize);
    midHighXoverSlider.setBounds(lowMidXoverSlider.getRight() + 5, xoverY, xOverSize, xOverSize);

    oversamplingSelection.setBounds(overX, overY, overW, overH);
}