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

ClipperBandControls::ClipperBandControls(juce::AudioProcessorValueTreeState& apvts) : apvts(apvts)
{

    bandClipSlider.setSliderStyle(juce::Slider::LinearVertical);
    bandClipSlider.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);

    bandGainSlider.setSliderStyle(juce::Slider::LinearVertical);
    bandGainSlider.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);


    addAndMakeVisible(bandGainSlider);
    addAndMakeVisible(bandClipSlider);

    bypassButton.setName("bypass");
    soloButton.setName("solo");
    muteButton.setName("mute");

    addAndMakeVisible(bypassButton);
    addAndMakeVisible(soloButton);
    addAndMakeVisible(muteButton);

    //lowBandButton.setName("l");
    //midBandButton.setName("m");
    //highBandButton.setName("h");

    //lowBandButton.setRadioGroupId(1);
    //midBandButton.setRadioGroupId(1);
    //highBandButton.setRadioGroupId(1);

    auto buttonSwitcher = [safePtr = this->safePtr]()
    {
        if (auto* component = safePtr.getComponent())
        {
            component->updateAttachments();
        }
    };

    lowBandButton.onClick = buttonSwitcher;
    midBandButton.onClick = buttonSwitcher;
    highBandButton.onClick = buttonSwitcher;

    lowBandButton.setToggleState(true, juce::NotificationType::dontSendNotification);

    updateAttachments();

    //addAndMakeVisible(lowBandButton);
    //addAndMakeVisible(midBandButton);
    //addAndMakeVisible(highBandButton);


}

void ClipperBandControls::updateAttachments()
{
    enum BandType
    {
        Low,
        Mid,
        High
    };

    BandType bandType = [this]()
    {
        if (lowBandButton.getToggleState())
            return BandType::Low;
        if (midBandButton.getToggleState())
            return BandType::Mid;
        return BandType::High;
    }();

    using namespace Params;
    std::vector<Names> names;

    switch (bandType)
    {
    case Low:
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

    case Mid:
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

    case High:
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

    const auto& params = GetParams();

    auto getParamHelper = [&params, &apvts = this->apvts, &names](const auto& position) -> auto&
    {
        return getParam(apvts, params, names.at(position));
    };

    bandGainSliderAttachment.reset();
    bandClipSliderAttachment.reset();
    bypassButtonAttachment.reset();
    muteButtonAttachment.reset();
    soloButtonAttachment.reset();

    //auto& gainParam = getParamHelper(Position::Gain);
    //bandGainSlider.changeParam(&gainParam);
    //auto& clipParam = getParamHelper(Position::Clip);
    //bandClipSlider.changeParam(&clipParam);


    auto makeAttachmentHelper = [&params, &apvts = this->apvts](auto& attachment, const auto& name, auto& slider)
    {
        makeAttachment(attachment, apvts, params, name, slider);
    };

    makeAttachmentHelper(bandGainSliderAttachment, names[Position::Gain], bandGainSlider);
    makeAttachmentHelper(bandClipSliderAttachment, names[Position::Clip], bandClipSlider);

    makeAttachmentHelper(bypassButtonAttachment, names[Position::Bypassed], bypassButton);
    makeAttachmentHelper(soloButtonAttachment, names[Position::Solo], soloButton);
    makeAttachmentHelper(muteButtonAttachment, names[Position::Mute], muteButton);
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
