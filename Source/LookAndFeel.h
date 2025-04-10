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

struct ChompLookAndFeel : juce::LookAndFeel_V4
{
    void drawToggleButton(juce::Graphics& g,
        juce::ToggleButton& toggleButton,
        bool shouldDrawButtonAsHighlighted,
        bool shouldDrawButtonAsDown) override;

    void drawButtonBackground(juce::Graphics& g, juce::Button& button, const juce::Colour& backgroundColour, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override;
    void drawButtonText(juce::Graphics& g, juce::TextButton& button, bool, bool) override;

    void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height, float sliderPos,
        const float rotaryStartAngle, const float rotaryEndAngle, juce::Slider& slider) override;
    void drawLinearSlider(juce::Graphics& g, int x, int y, int width, int height,
        float sliderPos,
        float minSliderPos,
        float maxSliderPos,
        const juce::Slider::SliderStyle style, juce::Slider& slider) override;

    void drawComboBox(juce::Graphics&, int width, int height, bool isButtonDown,
        int buttonX, int buttonY, int buttonW, int buttonH,
        juce::ComboBox&) override;
    void drawPopupMenuBackground(juce::Graphics& g, int width, int height) override;

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
struct SettingsButton : juce::TextButton
{
    void paintButton(juce::Graphics& g, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override
    {
        auto bounds = getLocalBounds().toFloat();
        auto gearRadius = juce::jmin(bounds.getWidth(), bounds.getHeight()) * 0.4f; 
        auto center = bounds.getCentre();
        juce::Path gearPath;

        const int teethCount = 8;
        const float toothSize = juce::MathConstants<float>::twoPi / teethCount;

        for (int i = 0; i < teethCount; ++i)
        {
            float angle = toothSize * i;
            float nextAngle = toothSize * (i + 1);

            
            juce::Point<float> p1(center.x + cos(angle - toothSize * 0.2f) * gearRadius * 1.2f,
                center.y + sin(angle - toothSize * 0.2f) * gearRadius * 1.2f);
            juce::Point<float> p2(center.x + cos(angle + toothSize * 0.2f) * gearRadius * 1.2f,
                center.y + sin(angle + toothSize * 0.2f) * gearRadius * 1.2f);

            if (i == 0)
                gearPath.startNewSubPath(p1);
            else
                gearPath.lineTo(p1);

            gearPath.lineTo(p2);
            gearPath.addArc(center.x - gearRadius, center.y - gearRadius, gearRadius * 2, gearRadius * 2, angle, nextAngle);
        }

        gearPath.closeSubPath();

        g.setColour(juce::Colours::white);
        g.strokePath(gearPath, juce::PathStrokeType(2.0f));
    }
};
struct WaveSelector : juce::ComboBox
{
    juce::StringArray waveTypeNames = {
        "Hard",
        "Soft",
        "Smooth"
    };

    WaveSelector()
    {
        auto chompLAF = juce::SharedResourcePointer<ChompLookAndFeel>();
        setLookAndFeel(chompLAF);
        for (int i = 0; i < waveTypeNames.size(); i++)
        {
           addItem(waveTypeNames[i], i + 1);
        }

    }
};
