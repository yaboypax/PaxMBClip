/*
  ==============================================================================

    Utilities.cpp
    Created: 29 Aug 2023 8:13:05pm
    Author:  Paxton Fleming

  ==============================================================================
*/

#include "Utilities.h"


void drawModuleBackground(juce::Graphics& g, juce::Rectangle<int> bounds)
{
    g.setColour(juce::Colours::navajowhite);
    g.fillAll();

    auto localBounds = bounds;

    bounds.reduce(3, 3);
    g.setColour(juce::Colours::black);
    g.fillRoundedRectangle(bounds.toFloat(), 3);

    g.drawRect(localBounds);
}