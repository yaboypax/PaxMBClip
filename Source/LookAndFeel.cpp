/*
  ==============================================================================

    LookAndFeel.cpp
    Created: 29 Aug 2023 7:54:33pm
    Author:  Paxton Fleming

  ==============================================================================
*/

#include "LookAndFeel.h"

void ChompLookAndFeel::drawToggleButton(juce::Graphics& g,
    juce::ToggleButton& toggleButton,
    bool shouldDrawButtonAsHighlighted,
    bool shouldDrawButtonAsDown)
{
    using namespace juce;

    if (auto* pb = dynamic_cast<PowerButton*>(&toggleButton))
    {
        Path powerButton;

        auto bounds = toggleButton.getLocalBounds();

        auto size = jmin(bounds.getWidth(), bounds.getHeight()) - 6;
        auto r = bounds.withSizeKeepingCentre(size, size).toFloat();

        float ang = 30.f; //30.f;

        size -= 6;

        powerButton.addCentredArc(r.getCentreX(),
            r.getCentreY(),
            size * 0.5,
            size * 0.5,
            0.f,
            degreesToRadians(ang),
            degreesToRadians(360.f - ang),
            true);

        powerButton.startNewSubPath(r.getCentreX(), r.getY());
        powerButton.lineTo(r.getCentre());

        PathStrokeType pst(2.f, PathStrokeType::JointStyle::curved);

        auto color = toggleButton.getToggleState() ? Colours::dimgrey : juce::Colour(188, 198, 206);

        g.setColour(color);
        g.strokePath(powerButton, pst);
        g.drawEllipse(r, 2);
    }
    else if (auto* analyzerButton = dynamic_cast<AnalyzerButton*>(&toggleButton))
    {
        auto color = !toggleButton.getToggleState() ? Colours::dimgrey : juce::Colour(188, 198, 206);

        g.setColour(color);

        auto bounds = toggleButton.getLocalBounds();
        g.drawRect(bounds);

        g.strokePath(analyzerButton->randomPath, PathStrokeType(1.f));
    }
    else
    {
        auto bounds = toggleButton.getLocalBounds().reduced(2);
        bool buttonIsOn = toggleButton.getToggleState();

        const int cornerSize = 4;

        g.setColour(buttonIsOn ? juce::Colour(188, 198, 206) : juce::Colours::black);
        g.fillRoundedRectangle(bounds.toFloat(), cornerSize);

        g.setColour(buttonIsOn ? juce::Colours::black : juce::Colour(188, 198, 206));
        g.drawRoundedRectangle(bounds.toFloat(), cornerSize, 1);

        g.drawFittedText(toggleButton.getName(), bounds, juce::Justification::centred, 1);

    }
}


void ChompLookAndFeel::drawButtonBackground(juce::Graphics& g, juce::Button& button, const juce::Colour& backgroundColour, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown)
{

    auto cornerSize = 9.0f;
    auto bounds = button.getLocalBounds();
    g.setColour(juce::Colours::black);
    g.fillRoundedRectangle(bounds.toFloat(), cornerSize);
    bounds.reduce(2, 2);

    g.setColour(backgroundColour);
    g.fillRoundedRectangle(bounds.toFloat(), cornerSize);

}

void ChompLookAndFeel::drawButtonText(juce::Graphics& g, juce::TextButton& button, bool, bool)
{
    juce::Font font = Chomp::Overhead;
    g.setFont(font);
    g.setColour(juce::Colours::black);

    const int yIndent = juce::jmin(4, button.proportionOfHeight(2.0f));
    const int cornerSize = juce::jmin(button.getHeight(), button.getWidth()) / 2;

    const int fontHeight = juce::roundToInt(font.getHeight());
    const int leftIndent = juce::jmin(fontHeight, cornerSize / (button.isConnectedOnLeft() ? 4 : 2));
    const int rightIndent = juce::jmin(fontHeight, cornerSize / (button.isConnectedOnRight() ? 4 : 2));
    const int textWidth = button.getWidth() - leftIndent - rightIndent;

    if (textWidth > 0)
        g.drawFittedText(button.getButtonText(),
            leftIndent, yIndent + 5, textWidth, button.getHeight() - yIndent * 2,
            juce::Justification::centred, 2);
}

void ChompLookAndFeel::drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height, float sliderPos,
    const float rotaryStartAngle, const float rotaryEndAngle, juce::Slider& slider)
{
 /*   auto outline = slider.findColour(juce::Slider::rotarySliderOutlineColourId);
    auto fill = slider.findColour(juce::Slider::rotarySliderFillColourId);

    auto bounds = juce::Rectangle<int>(x, y, width, height).toFloat().reduced(10);

    auto radius = juce::jmin(bounds.getWidth(), bounds.getHeight()) / 2.0f;
    auto toAngle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);
    auto lineW = jmin(8.0f, radius * 0.5f);
    auto arcRadius = radius - lineW * 0.5f;

    g.setColour(outline);
    g.drawEllipse(bounds, 5);

    if (slider.isEnabled())
    {
        juce::Path valueArc;
        juce::valueArc.addCentredArc(bounds.getCentreX(),
            bounds.getCentreY(),
            arcRadius,
            arcRadius,
            0.0f,
            rotaryStartAngle,
            toAngle,
            true);
    }

    float halfLineLength = 10.0f;

    juce::Point<float> midPoint(bounds.getCentreX() + arcRadius * std::cos(toAngle - MathConstants<float>::halfPi),
        bounds.getCentreY() + arcRadius * std::sin(toAngle - MathConstants<float>::halfPi));
    Point<float> lineStart(midPoint.x - halfLineLength * std::cos(toAngle - MathConstants<float>::halfPi),
        midPoint.y - halfLineLength * std::sin(toAngle - MathConstants<float>::halfPi));
    Point<float> lineEnd(midPoint.x + halfLineLength * std::cos(toAngle - MathConstants<float>::halfPi),
        midPoint.y + halfLineLength * std::sin(toAngle - MathConstants<float>::halfPi));

    g.drawLine(Line<float>(lineStart, lineEnd), lineW);*/
}

void ChompLookAndFeel::drawLinearSlider(juce::Graphics& g, int x, int y, int width, int height,
    float sliderPos,
    float minSliderPos,
    float maxSliderPos,
    const juce::Slider::SliderStyle style, juce::Slider& slider)
{
    auto trackWidth = juce::jmin(6.0f, slider.isHorizontal() ? (float)height * 0.25f : (float)width * 0.25f);

    juce::Point<float> startPoint(slider.isHorizontal() ? (float)x : (float)x + (float)width * 0.5f,
        slider.isHorizontal() ? (float)y + (float)height * 0.5f : (float)(height + y));

    juce::Point<float> endPoint(slider.isHorizontal() ? (float)(width + x) : startPoint.x,
        slider.isHorizontal() ? startPoint.y : (float)y);

    juce::Path backgroundTrack;
    backgroundTrack.startNewSubPath(startPoint);
    backgroundTrack.lineTo(endPoint);
    g.setColour(juce::Colours::white);
    g.strokePath(backgroundTrack, { trackWidth, juce::PathStrokeType::curved, juce::PathStrokeType::rounded });

    juce::Path valueTrack;
    juce::Point<float> minPoint, maxPoint, thumbPoint;

    auto kx = slider.isHorizontal() ? sliderPos : ((float)x + (float)width * 0.5f);
    auto ky = slider.isHorizontal() ? ((float)y + (float)height * 0.5f) : sliderPos;

    minPoint = startPoint;
    maxPoint = { kx, ky };

    auto thumbWidth = getSliderThumbRadius(slider);

    valueTrack.startNewSubPath(minPoint);
    g.setColour(juce::Colours::white);
    g.strokePath(valueTrack, { trackWidth, juce::PathStrokeType::curved, juce::PathStrokeType::rounded });

    auto halfLineLength = 10.f;
    juce::Point<float> lineStart, lineEnd;

    if (slider.isHorizontal())
    {

        lineStart = { maxPoint.x, maxPoint.y - halfLineLength };
        lineEnd = { maxPoint.x, maxPoint.y + halfLineLength };
    }
    else
    {
        lineStart = { maxPoint.x - halfLineLength, maxPoint.y };
        lineEnd = { maxPoint.x + halfLineLength , maxPoint.y };
    }

    g.setColour(juce::Colours::white);
    g.drawLine(juce::Line<float>(lineStart, lineEnd), 5);
}
