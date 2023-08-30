/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
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
//
//struct RotarySliderWithLabels : juce::Slider
//{
//    RotarySliderWithLabels(juce::RangedAudioParameter& rap, const juce::String& unitSuffix) :
//        juce::Slider(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag,
//            juce::Slider::TextEntryBoxPosition::NoTextBox),
//        param(&rap),
//        suffix(unitSuffix)
//    {
//        setLookAndFeel(&lnf);
//    }
//
//    ~RotarySliderWithLabels()
//    {
//        setLookAndFeel(nullptr);
//    }
//
//    struct LabelPos
//    {
//        float pos;
//        juce::String label;
//    };
//
//    juce::Array<LabelPos> labels;
//
//    void paint(juce::Graphics& g) override;
//    juce::Rectangle<int> getSliderBounds() const;
//    int getTextHeight() const { return 14; }
//    juce::String getDisplayString() const;
//private:
//    LookAndFeel lnf;
//
//    juce::RangedAudioParameter* param;
//    juce::String suffix;
//};

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

struct Placeholder : juce::Component
{
    Placeholder();

    void paint(juce::Graphics& g) override
    {
        g.fillAll(customColor);
    }
    juce::Colour customColor;
};

struct RotarySlider : juce::Slider
{
    RotarySlider() : juce::Slider(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag,
        juce::Slider::TextEntryBoxPosition::NoTextBox) {}
};


template<typename Attachment, typename APVTS, typename Params, typename ParamName, typename SliderType>
void makeAttachment(std::unique_ptr<Attachment>& attachment, APVTS& apvts, const Params& params, const ParamName& name, SliderType& slider)
{
    attachment = std::make_unique<Attachment>(apvts, params.at(name), slider);
}

struct ClipperBandControls : juce::Component
{
    ClipperBandControls(juce::AudioProcessorValueTreeState& apvts);
    void paint(juce::Graphics& g) override;
    void resized() override;

private:
    LookAndFeel lnf;

    using Attachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    RotarySlider bandGainSlider, bandClipSlider;
    std::unique_ptr<Attachment> bandGainSliderAttachment, bandClipSliderAttachment;

    using BtnAttachment = juce::AudioProcessorValueTreeState::ButtonAttachment;
    juce::ToggleButton bypassButton, soloButton, muteButton;
    std::unique_ptr<BtnAttachment> bypassButtonAttachment, soloButtonAttachment, muteButtonAttachment;

    juce::ToggleButton lowBandButton, midBandButton, highBandButton;
    //std::unique_ptr<BtnAttachment> bypassButtonAttachment, soloButtonAttachment, muteButtonAttachment;
};

struct GlobalControls : juce::Component
{
    GlobalControls(juce::AudioProcessorValueTreeState& apvts);
    void paint(juce::Graphics& g) override;
    void resized() override;

private:
    RotarySlider inGainSlider, lowMidXoverSlider, midHighXoverSlider, outGainSlider;

    using Attachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    std::unique_ptr<Attachment> lowMidXoverSliderAttachment, midHighXoverSliderAttachment, inGainSliderAttachment, outGainSliderAttachment;

};


/**
*/
class PaxMBClipAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    PaxMBClipAudioProcessorEditor (PaxMBClipAudioProcessor&);
    ~PaxMBClipAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    PaxMBClipAudioProcessor& audioProcessor;

    Placeholder controlBar, analyzer; 

    GlobalControls globalControls {audioProcessor.apvts};
    ClipperBandControls bandControls {audioProcessor.apvts};

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PaxMBClipAudioProcessorEditor)
};
