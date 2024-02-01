/*
  ==============================================================================

    ClipperBandControls.cpp
    Created: 29 Aug 2023 8:06:37pm
    Author:  flemi

  ==============================================================================
*/

#include "ClipperBandControls.h"

namespace
{
   constexpr int buttonX = 6;
   constexpr int buttonY = 5;
   constexpr int buttonSize = 45;
   constexpr int margin = 4;

   constexpr int sliderX = 26;
   constexpr int sliderY = 53;
   constexpr int sliderWidth = 48;
   constexpr int sliderHeight = 400;
}

ClipperBandControls::ClipperBandControls(PaxMBClipAudioProcessor& inProcessor)
{
    m_processor = &inProcessor;
    
    layoutButtons();
    layoutSliders();
    updateAttachments();

}

void ClipperBandControls::layoutButtons()
{
    bypassButton.setName("bypass");
    soloButton.setName("solo");
    muteButton.setName("mute");

    addAndMakeVisible(bypassButton);
    addAndMakeVisible(soloButton);
    addAndMakeVisible(muteButton);

}

void ClipperBandControls::layoutSliders()
{
    bandClipSlider.setSliderStyle(juce::Slider::LinearVertical);
    bandClipSlider.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);

    bandGainSlider.setSliderStyle(juce::Slider::LinearVertical);
    bandGainSlider.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);

    addAndMakeVisible(bandGainSlider);
    addAndMakeVisible(bandClipSlider);
}

void ClipperBandControls::updateAttachments()
{

    if (m_processor)
    {
        m_focus = m_processor->getBandFocus();
    }
    else return;

    using namespace Params;
    std::vector<Names> names;
    const auto& params = GetParams();

    if (m_focus == BandFocus::unfocused)
        return;

    switch (m_focus)
    {
    case BandFocus::Low:
    {
        names = std::vector<Names>
        {
            Names::Low_Gain,
            Names::Low_Clip,
            Names::Bypassed_Low,
            Names::Solo_Low,
            Names::Mute_Low
        };
        break;
    }

    case BandFocus::Mid:
    {
        names = std::vector<Names>
        {
            Names::Mid_Gain,
            Names::Mid_Clip,
            Names::Bypassed_Mid,
            Names::Solo_Mid,
            Names::Mute_Mid
        };
        break;
    }

    case BandFocus::High:
    {
        names = std::vector<Names>
        {
            Names::High_Gain,
            Names::High_Clip,
            Names::Bypassed_High,
            Names::Solo_High,
            Names::Mute_High
        };
        break;
    }

    }

    enum Position
    {
        Gain,
        Clip,
        Bypassed,
        Solo,
        Mute
    };

    bandGainSliderAttachment.reset();
    bandClipSliderAttachment.reset();
    bypassButtonAttachment.reset();
    muteButtonAttachment.reset();
    soloButtonAttachment.reset();

    //auto& gainParam = getParam(m_processor->apvts, params, names.at(Position::Gain));
    //bandGainSlider.changeParam(&gainParam);
    //auto& clipParam = getParamHelper(Position::Clip);
    //bandClipSlider.changeParam(&clipParam);

    makeAttachment(bandGainSliderAttachment, m_processor->apvts, params, names[Position::Gain], bandGainSlider);
    makeAttachment(bandClipSliderAttachment, m_processor->apvts, params, names[Position::Clip], bandClipSlider);

    makeAttachment(bypassButtonAttachment, m_processor->apvts, params, names[Position::Bypassed], bypassButton);
    makeAttachment(soloButtonAttachment, m_processor->apvts, params, names[Position::Solo], soloButton);
    makeAttachment(muteButtonAttachment, m_processor->apvts, params, names[Position::Mute], muteButton);
}



void ClipperBandControls::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds();
    drawModuleBackground(g, bounds);
}

void ClipperBandControls::resized()
{
    bypassButton.setBounds(buttonX, buttonY, buttonSize, buttonSize);
    muteButton.setBounds(bypassButton.getRight() + margin, buttonY, buttonSize, buttonSize);
    soloButton.setBounds(muteButton.getRight() + margin, buttonY, buttonSize, buttonSize);

    bandGainSlider.setBounds(sliderX, sliderY, sliderWidth, sliderHeight);
    bandClipSlider.setBounds(bandGainSlider.getRight() + 5, sliderY, sliderWidth, sliderHeight);
}
