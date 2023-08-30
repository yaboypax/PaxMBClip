/*
  ==============================================================================

    ClipperBandControls.cpp
    Created: 29 Aug 2023 8:06:37pm
    Author:  flemi

  ==============================================================================
*/

#include "ClipperBandControls.h"

ClipperBandControls::ClipperBandControls(juce::AudioProcessorValueTreeState& apvts) : apvts(apvts)
{

    addAndMakeVisible(bandGainSlider);
    addAndMakeVisible(bandClipSlider);

    bypassButton.setName("x");
    soloButton.setName("s");
    muteButton.setName("m");

    addAndMakeVisible(bypassButton);
    addAndMakeVisible(soloButton);
    addAndMakeVisible(muteButton);

    bypassButton.setLookAndFeel(&lnf);
    soloButton.setLookAndFeel(&lnf);
    muteButton.setLookAndFeel(&lnf);

    lowBandButton.setName("l");
    midBandButton.setName("m");
    highBandButton.setName("h");

    lowBandButton.setRadioGroupId(1);
    midBandButton.setRadioGroupId(1);
    highBandButton.setRadioGroupId(1);

    lowBandButton.setLookAndFeel(&lnf);
    midBandButton.setLookAndFeel(&lnf);
    highBandButton.setLookAndFeel(&lnf);

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

    addAndMakeVisible(lowBandButton);
    addAndMakeVisible(midBandButton);
    addAndMakeVisible(highBandButton);


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

    auto& gainParam = getParamHelper(Position::Gain);
    bandGainSlider.changeParam(&gainParam);
    auto& clipParam = getParamHelper(Position::Clip);
    bandClipSlider.changeParam(&clipParam);
    //auto& bypassedParam = getParamHelper(Position::Bypassed);
    //auto& soloParam = getParamHelper(Position::Solo);
    //auto& muteParam = getParamHelper(Position::Mute);

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
    auto bounds = getLocalBounds();
    bounds.reduce(3, 6);

    auto createBandButtonControlBox = [](std::vector<Component*> components)
    {
        juce::FlexBox flexBox;
        flexBox.flexDirection = juce::FlexBox::Direction::column;
        flexBox.flexWrap = juce::FlexBox::Wrap::noWrap;

        auto spacer = juce::FlexItem().withWidth(2);

        for (auto* component : components)
        {
            flexBox.items.add(spacer);
            flexBox.items.add(juce::FlexItem(*component).withFlex(1.0f));
        }

        flexBox.items.add(spacer);

        return flexBox;
    };

    auto bandButtonControlBox = createBandButtonControlBox({ &bypassButton, &soloButton, &muteButton });
    auto bandSelectControlBox = createBandButtonControlBox({ &lowBandButton, &midBandButton, &highBandButton });


    juce::FlexBox flexBox;
    flexBox.flexDirection = juce::FlexBox::Direction::row;
    flexBox.flexWrap = juce::FlexBox::Wrap::noWrap;

    auto spacer = juce::FlexItem().withWidth(4);
    auto endCap = juce::FlexItem().withWidth(6);

    flexBox.items.add(spacer);
    flexBox.items.add(juce::FlexItem(bandSelectControlBox).withWidth(50));
    flexBox.items.add(spacer);
    flexBox.items.add(juce::FlexItem(bandGainSlider).withFlex(1.f));
    flexBox.items.add(spacer);
    flexBox.items.add(juce::FlexItem(bandClipSlider).withFlex(1.f));
    flexBox.items.add(spacer);
    flexBox.items.add(juce::FlexItem(bandButtonControlBox).withWidth(30));
    flexBox.items.add(endCap);

    flexBox.performLayout(bounds);
}
