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
    //constexpr int sliderHeight = 355;

    constexpr int overX = 26;
    constexpr int overY = 22;
    constexpr int overW = 100;
    constexpr int overH = 20;

    constexpr int xoverX = 16;
    //constexpr int xoverY = 405;
    constexpr int xOverSize = 58;
}



GlobalControls::GlobalControls(PaxMBClipAudioProcessor& inProcessor)
{
    m_processor = &inProcessor;

    using namespace Params;
    const auto& params = GetParams();

    makeAttachment(inGainSliderAttachment, m_processor->apvts, params, Names::Gain_In, inGainSlider);
    makeAttachment(lowMidXoverSliderAttachment, m_processor->apvts, params, Names::Low_Mid_Crossover_Freq, lowMidXoverSlider);
    makeAttachment(midHighXoverSliderAttachment, m_processor->apvts, params, Names::Mid_High_Crossover_Freq, midHighXoverSlider);
    makeAttachment(outGainSliderAttachment, m_processor->apvts, params, Names::Gain_Out, outGainSlider);

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

    masterClipButton.setButtonText("Master Clip");
    addAndMakeVisible(masterClipButton);

    masterClipButton.setClickingTogglesState(true);
    masterClipButton.onClick = [this] {
        auto param = m_processor->apvts.getParameter(Params::GetParams().at(Params::Names::Master_Clip));
        param->beginChangeGesture();
        param->setValueNotifyingHost(masterClipButton.getToggleState());
        param->endChangeGesture();
        };



}

void GlobalControls::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds();
    drawModuleBackground(g, bounds);
}

void GlobalControls::resized()
{
    const int sliderHeight = getHeight() - 113;
    inGainSlider.setBounds(sliderX, sliderY, sliderWidth, sliderHeight);
    outGainSlider.setBounds(inGainSlider.getRight() + 5, sliderY, sliderWidth, sliderHeight);

    const int xoverY = getHeight() - 63;
    lowMidXoverSlider.setBounds(xoverX, xoverY, xOverSize, xOverSize);
    midHighXoverSlider.setBounds(lowMidXoverSlider.getRight() + 5, xoverY, xOverSize, xOverSize);

    masterClipButton.setBounds(overX, overY, overW, overH);
}