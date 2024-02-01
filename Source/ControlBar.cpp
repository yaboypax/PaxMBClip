/*
  ==============================================================================

    ControlBar.cpp
    Created: 2 Sep 2023 7:39:03pm
    Author:  Paxton Fleming

  ==============================================================================
*/

#include "ControlBar.h"

ControlBar::ControlBar()
{
    analyzerButton.setToggleState(true, juce::NotificationType::dontSendNotification);
    addAndMakeVisible(analyzerButton);

    globalBypassButton.setToggleState(false, juce::NotificationType::dontSendNotification);
    addAndMakeVisible(globalBypassButton);
}

void ControlBar::paint(juce::Graphics& g)
{
    g.setColour(juce::Colours::white);
    g.setFont(18.f);
    g.drawText("Ya Boy Pax               CHOMP           Multiband Clipper", 100, 5, 500, 18, juce::Justification::centred, false);
}

void ControlBar::resized()
{
    auto bounds = getLocalBounds();
    analyzerButton.setBounds(getX() + 4, getY() + 4, 50, getHeight() - 8);
    globalBypassButton.setBounds(getWidth() - 54, getY() + 4, 50, getHeight() - 8);
}