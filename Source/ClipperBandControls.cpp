/*
  ==============================================================================

    ClipperBandControls.cpp
    Created: 29 Aug 2023 8:06:37pm
    Author:  flemi

  ==============================================================================
*/

#include "ClipperBandControls.h"

namespace
{
    constexpr int buttonX = 6;
    constexpr int buttonY = 5;
    constexpr int buttonSize = 45;
    constexpr int margin = 4;

    constexpr int sliderX = 26;
    constexpr int sliderY = 53;
    constexpr int sliderWidth = 48;
    //constexpr int sliderHeight = 355;

    constexpr int waveX = 26;
    //constexpr int waveY = 422;
    constexpr int waveW = 100;
    constexpr int waveH = 20;
}

ClipperBandControls::ClipperBandControls(PaxMBClipAudioProcessor& inProcessor)
{
    m_processor = &inProcessor;
    m_processor->addChangeListener(this);

    layoutButtons();
    layoutSliders();

    addAndMakeVisible(m_clipWave);

    updateAttachments();

}

ClipperBandControls::~ClipperBandControls()
{
    m_processor->removeChangeListener(this);
}

void ClipperBandControls::layoutButtons()
{
    bypassButton.setName("bypass");
    soloButton.setName("solo");
    muteButton.setName("mute");

    addAndMakeVisible(bypassButton);
    addAndMakeVisible(soloButton);
    addAndMakeVisible(muteButton);

}

void ClipperBandControls::layoutSliders()
{
    bandClipSlider.setSliderStyle(juce::Slider::LinearVertical);
    bandClipSlider.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);

    bandGainSlider.setSliderStyle(juce::Slider::LinearVertical);
    bandGainSlider.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);

    bandGainSlider.onValueChange = [this] {
        DBG(bandGainSlider.getValue());
        };
    addAndMakeVisible(bandGainSlider);
    addAndMakeVisible(bandClipSlider);
}

void ClipperBandControls::updateAttachments()
{

    if (m_processor)
    {
        m_focus = m_processor->getBandFocus();
    }
    else return;

    using namespace Params;
    std::vector<Names> names;
    const auto& params = GetParams();



    if (m_focus == BandFocus::unfocused)
        return;

    switch (m_focus)
    {
    case BandFocus::Low:
    {
        names = std::vector<Names>
        {
            Names::Low_Gain,
            Names::Low_Clip,
            Names::Bypassed_Low,
            Names::Solo_Low,
            Names::Mute_Low,
            Names::Low_Wave
        };
        break;
    }

    case BandFocus::Mid:
    {
        names = std::vector<Names>
        {
            Names::Mid_Gain,
            Names::Mid_Clip,
            Names::Bypassed_Mid,
            Names::Solo_Mid,
            Names::Mute_Mid,
            Names::Mid_Wave
        };
        break;
    }

    case BandFocus::High:
    {
        names = std::vector<Names>
        {
            Names::High_Gain,
            Names::High_Clip,
            Names::Bypassed_High,
            Names::Solo_High,
            Names::Mute_High,
            Names::High_Wave
        };
        break;
    }

    }

    enum Position
    {
        Gain,
        Clip,
        Bypassed,
        Solo,
        Mute,
        Wave
    };

    bandGainSliderAttachment.reset();
    bandClipSliderAttachment.reset();

    bypassButtonAttachment.reset();
    muteButtonAttachment.reset();
    soloButtonAttachment.reset();

    m_clipWaveAttachment.reset();

    makeAttachment(m_clipWaveAttachment, m_processor->apvts, params, names[Position::Wave], m_clipWave);

    makeAttachment(bandGainSliderAttachment, m_processor->apvts, params, names[Position::Gain], bandGainSlider);
    makeAttachment(bandClipSliderAttachment, m_processor->apvts, params, names[Position::Clip], bandClipSlider);

    makeAttachment(bypassButtonAttachment, m_processor->apvts, params, names[Position::Bypassed], bypassButton);
    makeAttachment(soloButtonAttachment, m_processor->apvts, params, names[Position::Solo], soloButton);
    makeAttachment(muteButtonAttachment, m_processor->apvts, params, names[Position::Mute], muteButton);

    bandClipSlider.setRange(-48.0, 0.0, 0.1);
    bandGainSlider.setRange(-24.0, 24.0, 0.1);
}



void ClipperBandControls::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds();
    drawModuleBackground(g, bounds);
}

void ClipperBandControls::resized()
{
    bypassButton.setBounds(buttonX, buttonY, buttonSize, buttonSize);
    muteButton.setBounds(bypassButton.getRight() + margin, buttonY, buttonSize, buttonSize);
    soloButton.setBounds(muteButton.getRight() + margin, buttonY, buttonSize, buttonSize);

    const int sliderHeight = getHeight() - 113;
    bandGainSlider.setBounds(sliderX, sliderY, sliderWidth, sliderHeight);
    bandClipSlider.setBounds(bandGainSlider.getRight() + 5, sliderY, sliderWidth, sliderHeight);

    const int waveY = getHeight() - 46;
    m_clipWave.setBounds(waveX, waveY, waveW, waveH);
}

void ClipperBandControls::changeListenerCallback(juce::ChangeBroadcaster* source)
{
    updateAttachments();
    resized();
    repaint();
}
