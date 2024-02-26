/*
  ==============================================================================

    ControlBar.cpp
    Created: 2 Sep 2023 7:39:03pm
    Author:  Paxton Fleming

  ==============================================================================
*/

#include "ControlBar.h"

ControlBar::ControlBar(PaxMBClipAudioProcessor& inProcessor)
{
    m_processor = &inProcessor;
    analyzerButton.setToggleState(true, juce::NotificationType::dontSendNotification);
    addAndMakeVisible(analyzerButton);

    globalBypassButton.setToggleState(false, juce::NotificationType::dontSendNotification);
    addAndMakeVisible(globalBypassButton);


    using namespace Params;
    const auto& params = GetParams();
    for (int i = 0; i <= 5; ++i)
    {
        oversamplingSelection.addItem(getOversamplingPower(i), i + 1);
    }

    makeAttachment(oversamplingAttachment, m_processor->apvts, params, Names::Oversample, oversamplingSelection);
    addAndMakeVisible(oversamplingSelection);
}

void ControlBar::paint(juce::Graphics& g)
{
    g.setColour(juce::Colours::white);
    g.setFont(Chomp::Overhead.withHeight(38.f));
    g.drawText("Ya Boy Pax    CHOMP  Multiband Clipper", 50, 2, 500, 18, juce::Justification::centred, false);

    juce::Rectangle<float> iconBounds = { 183.f, 2.f, 100.f, 27.5f };
    icon.drawWithin(g, iconBounds, juce::Justification::centred, 1.f);
}

void ControlBar::resized()
{
    auto bounds = getLocalBounds();
    analyzerButton.setBounds(getX() + 4, getY() + 4, 50, getHeight() - 8);
    globalBypassButton.setBounds(getWidth() - 54, getY() + 4, 50, getHeight() - 8);
    oversamplingSelection.setBounds(globalBypassButton.getX() - 150, 4, 120, getHeight() - 8);
}