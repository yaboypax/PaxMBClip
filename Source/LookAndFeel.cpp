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
        auto buttonText = toggleButton.getButtonText();

        const int cornerSize = 4;

        if (buttonText == "s")
        {
            g.setColour(buttonIsOn ? juce::Colours::yellow : juce::Colours::black);
        }
        else if (buttonText == "m")
        {
            g.setColour(buttonIsOn ? juce::Colours::red : juce::Colours::black);
        }
        
        g.fillRoundedRectangle(bounds.toFloat(), cornerSize);

        g.setColour(buttonIsOn ? juce::Colours::black : juce::Colour(188, 198, 206));
        g.drawRoundedRectangle(bounds.toFloat(), cornerSize, 1);

        g.drawFittedText(buttonText, bounds, juce::Justification::centred, 1);

    }
}


void ChompLookAndFeel::drawButtonBackground(juce::Graphics& g, juce::Button& button, const juce::Colour& backgroundColour, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown)
{

    auto cornerSize = 2.0f;
    auto bounds = button.getLocalBounds();

    g.setColour(button.getToggleState() ? juce::Colour(0xFFFF817F) : juce::Colours::black);
    g.fillRoundedRectangle(bounds.toFloat(), cornerSize);

}

void ChompLookAndFeel::drawButtonText(juce::Graphics& g, juce::TextButton& button, bool, bool)
{
    auto bounds = button.getLocalBounds();
    bounds.setY(bounds.getY() - 4);
    juce::Font font = Chomp::Overhead;
    g.setFont(font.withHeight(32.f));
    g.setColour(button.getToggleState() ? juce::Colours::black : juce::Colours::white);
    g.drawFittedText(button.getButtonText(), bounds, juce::Justification::centred, 1);
}

void ChompLookAndFeel::drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height, float sliderPos,
    const float rotaryStartAngle, const float rotaryEndAngle, juce::Slider& slider)
{
    auto outline = juce::Colours::white;
    auto fill = juce::Colours::transparentBlack;

    auto bounds = juce::Rectangle<int>(x, y, width, height).toFloat().reduced(10);

    auto radius = juce::jmin(bounds.getWidth(), bounds.getHeight()) / 2.0f;
    auto toAngle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);
    auto lineW = juce::jmin(8.0f, radius * 0.5f);
    auto arcRadius = radius - lineW * 0.5f;

    g.setColour(outline);
    g.drawEllipse(bounds, 5);

    if (slider.isEnabled())
    {
        juce::Path valueArc;
        valueArc.addCentredArc(bounds.getCentreX(),
            bounds.getCentreY(),
            arcRadius,
            arcRadius,
            0.0f,
            rotaryStartAngle,
            toAngle,
            true);
    }

    float halfLineLength = 10.0f;

    juce::Point<float> midPoint(bounds.getCentreX() + arcRadius * std::cos(toAngle - juce::MathConstants<float>::halfPi),
        bounds.getCentreY() + arcRadius * std::sin(toAngle - juce::MathConstants<float>::halfPi));
    juce::Point<float> lineStart(midPoint.x - halfLineLength * std::cos(toAngle - juce::MathConstants<float>::halfPi),
        midPoint.y - halfLineLength * std::sin(toAngle - juce::MathConstants<float>::halfPi));
    juce::Point<float> lineEnd(midPoint.x + halfLineLength * std::cos(toAngle - juce::MathConstants<float>::halfPi),
        midPoint.y + halfLineLength * std::sin(toAngle - juce::MathConstants<float>::halfPi));

    g.drawLine(juce::Line<float>(lineStart, lineEnd), lineW);
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

void ChompLookAndFeel::drawComboBox(juce::Graphics& g, int width, int height, bool,
    int, int, int, int, juce::ComboBox& box)
{
    auto cornerSize = box.findParentComponentOfClass<juce::ChoicePropertyComponent>() != nullptr ? 0.0f : 3.0f;
    juce::Rectangle<int> boxBounds(0, 0, width, height);

    g.setColour(juce::Colours::black);
    g.fillRoundedRectangle(boxBounds.toFloat(), cornerSize);

    g.setColour(juce::Colours::white);
    g.drawRoundedRectangle(boxBounds.toFloat().reduced(0.5f, 0.5f), cornerSize, 1.0f);

    juce::Rectangle<int> arrowZone(width - 30, 0, 20, height);
    juce::Path path;
    path.startNewSubPath((float)arrowZone.getX() + 3.0f, (float)arrowZone.getCentreY() - 2.0f);
    path.lineTo((float)arrowZone.getCentreX(), (float)arrowZone.getCentreY() + 3.0f);
    path.lineTo((float)arrowZone.getRight() - 3.0f, (float)arrowZone.getCentreY() - 2.0f);

    g.setColour(juce::Colours::white.withAlpha((box.isEnabled() ? 0.9f : 0.2f)));
    g.strokePath(path, juce::PathStrokeType(2.0f));
}

void ChompLookAndFeel::drawPopupMenuBackground(juce::Graphics& g, int width, int height)
{

    g.fillAll(juce::Colours::black);
    g.setColour(juce::Colours::white);
    g.drawRect(0, 0, width, height, 2);
}
