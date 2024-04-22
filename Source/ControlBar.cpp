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

    globalBypassButton.setToggleState(false, juce::NotificationType::dontSendNotification);
    addAndMakeVisible(globalBypassButton);
    
    addAndMakeVisible(settingsButton);
}

void ControlBar::paint(juce::Graphics& g)
{
    g.setColour(juce::Colours::white);
    g.setFont(Chomp::Overhead.withHeight(38.f));
    g.drawText("Ya Boy Pax", 10, 2, getWidth(), 18, juce::Justification::left, false);

    g.drawText("CHOMP", 10, 2, getWidth(), 18, juce::Justification::centred, false);

    g.drawText("Multiband Clipper", 10, 2, getWidth() - 120, 18, juce::Justification::centredRight, false);

    juce::Rectangle<float> iconBounds = { static_cast<float>(getWidth()) / 2.f - 100.f, 2.f, 100.f, 27.5f };
    icon.drawWithin(g, iconBounds, juce::Justification::centred, 1.f);
}

void ControlBar::resized()
{
    globalBypassButton.setBounds(getWidth() - 54, getY() + 4, 50, getHeight() - 8);
    settingsButton.setBounds(globalBypassButton.getX() - 44, getY() + 4, 50, getHeight() - 8);
}
