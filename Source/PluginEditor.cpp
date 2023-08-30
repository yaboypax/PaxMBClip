/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"


//void LookAndFeel::drawRotarySlider(juce::Graphics& g,
//    int x,
//    int y,
//    int width,
//    int height,
//    float sliderPosProportional,
//    float rotaryStartAngle,
//    float rotaryEndAngle,
//    juce::Slider& slider)
//{
//    using namespace juce;
//
//    auto bounds = Rectangle<float>(x, y, width, height);
//
//    auto enabled = slider.isEnabled();
//
//    g.setColour(enabled ? Colour(97u, 18u, 167u) : Colours::darkgrey);
//    g.fillEllipse(bounds);
//
//    g.setColour(enabled ? Colour(255u, 154u, 1u) : Colours::grey);
//    g.drawEllipse(bounds, 1.f);
//
//    if (auto* rswl = dynamic_cast<RotarySliderWithLabels*>(&slider))
//    {
//        auto center = bounds.getCentre();
//        Path p;
//
//        Rectangle<float> r;
//        r.setLeft(center.getX() - 2);
//        r.setRight(center.getX() + 2);
//        r.setTop(bounds.getY());
//        r.setBottom(center.getY() - rswl->getTextHeight() * 1.5);
//
//        p.addRoundedRectangle(r, 2.f);
//
//        jassert(rotaryStartAngle < rotaryEndAngle);
//
//        auto sliderAngRad = jmap(sliderPosProportional, 0.f, 1.f, rotaryStartAngle, rotaryEndAngle);
//
//        p.applyTransform(AffineTransform().rotated(sliderAngRad, center.getX(), center.getY()));
//
//        g.fillPath(p);
//
//        g.setFont(rswl->getTextHeight());
//        auto text = rswl->getDisplayString();
//        auto strWidth = g.getCurrentFont().getStringWidth(text);
//
//        r.setSize(strWidth + 4, rswl->getTextHeight() + 2);
//        r.setCentre(bounds.getCentre());
//
//        g.setColour(enabled ? Colours::black : Colours::darkgrey);
//        g.fillRect(r);
//
//        g.setColour(enabled ? Colours::white : Colours::lightgrey);
//        g.drawFittedText(text, r.toNearestInt(), juce::Justification::centred, 1);
//    }
//}

void LookAndFeel::drawToggleButton(juce::Graphics& g,
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

        auto color = toggleButton.getToggleState() ? Colours::dimgrey : Colour(0u, 172u, 1u);

        g.setColour(color);
        g.strokePath(powerButton, pst);
        g.drawEllipse(r, 2);
    }
    else if (auto* analyzerButton = dynamic_cast<AnalyzerButton*>(&toggleButton))
    {
        auto color = !toggleButton.getToggleState() ? Colours::dimgrey : Colour(0u, 172u, 1u);

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

        g.setColour(buttonIsOn ? juce::Colours::navajowhite : juce::Colours::black);
        g.fillRoundedRectangle(bounds.toFloat(), cornerSize);

        g.setColour(buttonIsOn ? juce::Colours::black : juce::Colours::navajowhite);
        g.drawRoundedRectangle(bounds.toFloat(), cornerSize, 1);

        g.drawFittedText(toggleButton.getName(), bounds, juce::Justification::centred, 1);

    }
}
//==============================================================================
//void RotarySliderWithLabels::paint(juce::Graphics& g)
//{
//    using namespace juce;
//
//    auto startAng = degreesToRadians(180.f + 45.f);
//    auto endAng = degreesToRadians(180.f - 45.f) + MathConstants<float>::twoPi;
//
//    auto range = getRange();
//
//    auto sliderBounds = getSliderBounds();
//
//    getLookAndFeel().drawRotarySlider(g,
//        sliderBounds.getX(),
//        sliderBounds.getY(),
//        sliderBounds.getWidth(),
//        sliderBounds.getHeight(),
//        jmap(getValue(), range.getStart(), range.getEnd(), 0.0, 1.0),
//        startAng,
//        endAng,
//        *this);
//
//    auto center = sliderBounds.toFloat().getCentre();
//    auto radius = sliderBounds.getWidth() * 0.5f;
//
//    g.setColour(Colour(0u, 172u, 1u));
//    g.setFont(getTextHeight());
//
//    auto numChoices = labels.size();
//    for (int i = 0; i < numChoices; ++i)
//    {
//        auto pos = labels[i].pos;
//        jassert(0.f <= pos);
//        jassert(pos <= 1.f);
//
//        auto ang = jmap(pos, 0.f, 1.f, startAng, endAng);
//
//        auto c = center.getPointOnCircumference(radius + getTextHeight() * 0.5f + 1, ang);
//
//        Rectangle<float> r;
//        auto str = labels[i].label;
//        r.setSize(g.getCurrentFont().getStringWidth(str), getTextHeight());
//        r.setCentre(c);
//        r.setY(r.getY() + getTextHeight());
//
//        g.drawFittedText(str, r.toNearestInt(), juce::Justification::centred, 1);
//    }
//
//}
//
//juce::Rectangle<int> RotarySliderWithLabels::getSliderBounds() const
//{
//    auto bounds = getLocalBounds();
//
//    auto size = juce::jmin(bounds.getWidth(), bounds.getHeight());
//
//    size -= getTextHeight() * 2;
//    juce::Rectangle<int> r;
//    r.setSize(size, size);
//    r.setCentre(bounds.getCentreX(), 0);
//    r.setY(2);
//
//    return r;
//
//}
//
//juce::String RotarySliderWithLabels::getDisplayString() const
//{
//    if (auto* choiceParam = dynamic_cast<juce::AudioParameterChoice*>(param))
//        return choiceParam->getCurrentChoiceName();
//
//    juce::String str;
//    bool addK = false;
//
//    if (auto* floatParam = dynamic_cast<juce::AudioParameterFloat*>(param))
//    {
//        float val = getValue();
//
//        if (val > 999.f)
//        {
//            val /= 1000.f; //1001 / 1000 = 1.001
//            addK = true;
//        }
//
//        str = juce::String(val, (addK ? 2 : 0));
//    }
//    else
//    {
//        jassertfalse; //this shouldn't happen!
//    }
//
//    if (suffix.isNotEmpty())
//    {
//        str << " ";
//        if (addK)
//            str << "k";
//
//        str << suffix;
//    }
//
//    return str;
//}

Placeholder::Placeholder()
{
    juce::Random r;
    customColor = juce::Colour(r.nextInt(255), r.nextInt(255), r.nextInt(255));
}

ClipperBandControls::ClipperBandControls(juce::AudioProcessorValueTreeState& apvts)
{
    using namespace Params;
    const auto& params = GetParams();

    auto makeAttachmentHelper = [&params, &apvts](auto& attachment, const auto& name, auto& slider)
    {
        makeAttachment(attachment, apvts, params, name, slider);
    };

    makeAttachmentHelper(bandGainSliderAttachment, Names::Mid_Gain, bandGainSlider);
    makeAttachmentHelper(bandClipSliderAttachment, Names::Mid_Clip, bandClipSlider);


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

}

void drawModuleBackground(juce::Graphics &g, juce::Rectangle<int> bounds)
{
    g.setColour(juce::Colours::navajowhite);
    g.fillAll();

    auto localBounds = bounds;

    bounds.reduce(3, 3);
    g.setColour(juce::Colours::black);
    g.fillRoundedRectangle(bounds.toFloat(), 3);

    g.drawRect(localBounds);
}

void ClipperBandControls::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds();
    drawModuleBackground(g, bounds);
}

void ClipperBandControls::resized()
{
    auto bounds = getLocalBounds();

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

    auto bandButtonControlBox = createBandButtonControlBox({&bypassButton, &soloButton, &muteButton});


    juce::FlexBox flexBox;
    flexBox.flexDirection = juce::FlexBox::Direction::row;
    flexBox.flexWrap = juce::FlexBox::Wrap::noWrap;

    auto spacer = juce::FlexItem().withWidth(4);
    auto endCap = juce::FlexItem().withWidth(6);

    flexBox.items.add(endCap);
    flexBox.items.add(juce::FlexItem(bandGainSlider).withFlex(1.f));
    flexBox.items.add(spacer);
    flexBox.items.add(juce::FlexItem(bandClipSlider).withFlex(1.f));
    flexBox.items.add(spacer);
    flexBox.items.add(juce::FlexItem(bandButtonControlBox).withWidth(30));
    flexBox.items.add(endCap);

    flexBox.performLayout(bounds);
}

GlobalControls::GlobalControls(juce::AudioProcessorValueTreeState& apvts)
{
    using namespace Params;
    const auto& params = GetParams();

    auto makeAttachmentHelper = [&params, &apvts](auto& attachment, const auto& name, auto& slider)
    {
        makeAttachment(attachment, apvts, params, name, slider);
    };

    makeAttachmentHelper(inGainSliderAttachment, Names::Gain_In, inGainSlider);
    makeAttachmentHelper(lowMidXoverSliderAttachment, Names::Low_Mid_Crossover_Freq, lowMidXoverSlider);
    makeAttachmentHelper(midHighXoverSliderAttachment, Names::Mid_High_Crossover_Freq, midHighXoverSlider);
    makeAttachmentHelper(inGainSliderAttachment, Names::Gain_In, inGainSlider);
    makeAttachmentHelper(outGainSliderAttachment, Names::Gain_Out, outGainSlider);

    addAndMakeVisible(inGainSlider);
    addAndMakeVisible(lowMidXoverSlider);
    addAndMakeVisible(midHighXoverSlider);
    addAndMakeVisible(outGainSlider);
}

void GlobalControls::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds();
    drawModuleBackground(g, bounds);
}

void GlobalControls::resized()
{
    auto bounds = getLocalBounds();

    juce::FlexBox flexBox;
    flexBox.flexDirection = juce::FlexBox::Direction::row;
    flexBox.flexWrap = juce::FlexBox::Wrap::noWrap;

    auto spacer = juce::FlexItem().withWidth(4);
    auto endCap = juce::FlexItem().withWidth(6);

    flexBox.items.add(endCap);
    flexBox.items.add(juce::FlexItem(inGainSlider).withFlex(1.f));
    flexBox.items.add(spacer);
    flexBox.items.add(juce::FlexItem(lowMidXoverSlider).withFlex(1.f));
    flexBox.items.add(spacer);
    flexBox.items.add(juce::FlexItem(midHighXoverSlider).withFlex(1.f));
    flexBox.items.add(spacer);
    flexBox.items.add(juce::FlexItem(outGainSlider).withFlex(1.f));
    flexBox.items.add(endCap);

    flexBox.performLayout(bounds);
}

//==============================================================================
PaxMBClipAudioProcessorEditor::PaxMBClipAudioProcessorEditor (PaxMBClipAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    //addAndMakeVisible(controlBar);
    //addAndMakeVisible(analyzer);
    addAndMakeVisible(globalControls);
    addAndMakeVisible(bandControls);

    setSize (600, 500);
}

PaxMBClipAudioProcessorEditor::~PaxMBClipAudioProcessorEditor()
{

}

//==============================================================================
void PaxMBClipAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll(juce::Colours::black);
}

void PaxMBClipAudioProcessorEditor::resized()
{
    auto bounds = getLocalBounds();

    controlBar.setBounds(bounds.removeFromTop(32));
    bandControls.setBounds(bounds.removeFromBottom(135));
    analyzer.setBounds(bounds.removeFromTop(225));
    globalControls.setBounds(bounds);
}
