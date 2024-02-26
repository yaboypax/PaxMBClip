/*
  ==============================================================================

    LookAndFeel.h
    Created: 29 Aug 2023 7:54:33pm
    Author:  Paxton Fleming

  ==============================================================================
*/

#pragma once
#include "JuceHeader.h"

namespace Chomp
{
    static auto typeface = juce::Typeface::createSystemTypefaceFor(BinaryData::Overhead_ttf, BinaryData::Overhead_ttfSize);
    static juce::Font Overhead = juce::Font(typeface);

    static juce::DrawableImage icon = juce::DrawableImage(juce::ImageCache::getFromMemory(BinaryData::chomp_icon_png, BinaryData::chomp_icon_pngSize));
};

static void drawModuleBackground(juce::Graphics& g, juce::Rectangle<int> bounds)
{
    g.setColour(juce::Colour(188, 198, 206));
    g.fillAll();

    auto localBounds = bounds;

    bounds.reduce(3, 3);
    g.setColour(juce::Colours::black);
    g.fillRoundedRectangle(bounds.toFloat(), 3);

    g.drawRect(localBounds);
}

struct LookAndFeel : juce::LookAndFeel_V4
{
    //void drawRotarySlider(juce::Graphics&,
    //    int x, int y, int width, int height,
    //    float sliderPosProportional,
    //    float rotaryStartAngle,
    //    float rotaryEndAngle,
    //    juce::Slider&) override;

    void drawToggleButton(juce::Graphics& g,
        juce::ToggleButton& toggleButton,
        bool shouldDrawButtonAsHighlighted,
        bool shouldDrawButtonAsDown) override;

};

struct PowerButton : juce::ToggleButton { };

struct AnalyzerButton : juce::ToggleButton
{
    void resized() override
    {
        auto bounds = getLocalBounds();
        auto insetRect = bounds.reduced(4);

        randomPath.clear();

        juce::Random r;

        randomPath.startNewSubPath(insetRect.getX(),
            insetRect.getY() + insetRect.getHeight() * r.nextFloat());

        for (auto x = insetRect.getX() + 1; x < insetRect.getRight(); x += 2)
        {
            randomPath.lineTo(x,
                insetRect.getY() + insetRect.getHeight() * r.nextFloat());
        }
    }

    juce::Path randomPath;
};


