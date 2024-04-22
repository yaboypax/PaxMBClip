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
    constexpr int rotaryX = 18;
    constexpr int rotaryY = 5;
    constexpr int rotarySize = 58;
    constexpr int margin = 4;

    constexpr int sliderX = 26;
    constexpr int sliderY = 55;
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

    layoutSliders();

    addAndMakeVisible(m_clipWave);

    updateAttachments();

}

ClipperBandControls::~ClipperBandControls()
{
    m_processor->removeChangeListener(this);
}

void ClipperBandControls::layoutSliders()
{
    bandClipSlider.setSliderStyle(juce::Slider::LinearVertical);
    bandClipSlider.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);

    bandGainSlider.setSliderStyle(juce::Slider::LinearVertical);
    bandGainSlider.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);

    addAndMakeVisible(bandGainSlider);
    addAndMakeVisible(bandClipSlider);

    addAndMakeVisible(m_lowMidSlider);
    addAndMakeVisible(m_midHighSlider);

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

    m_clipWaveAttachment.reset();

    makeAttachment(m_clipWaveAttachment, m_processor->apvts, params, names[Position::Wave], m_clipWave);

    makeAttachment(bandGainSliderAttachment, m_processor->apvts, params, names[Position::Gain], bandGainSlider);
    makeAttachment(bandClipSliderAttachment, m_processor->apvts, params, names[Position::Clip], bandClipSlider);

    makeAttachment(m_lowMidAttachment, m_processor->apvts, params, Names::Low_Mid_Crossover_Freq, m_lowMidSlider);
    makeAttachment(m_midHighAttachment, m_processor->apvts, params, Names::Mid_High_Crossover_Freq, m_midHighSlider);

    bandClipSlider.setRange(-48.0, 0.0, 0.1);
    bandGainSlider.setRange(-24.0, 24.0, 0.1);

    m_lowMidSlider.onValueChange = [this]() {
        m_processor->sendChangeMessage();
        };

    m_midHighSlider.onValueChange = [this]() {
        m_processor->sendChangeMessage();
        };
}



void ClipperBandControls::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds();
    drawModuleBackground(g, bounds);
}

void ClipperBandControls::resized()
{
    
    if (m_focus == BandFocus::unfocused)
        return;
    //bypassButton.setBounds(buttonX, buttonY, buttonSize, buttonSize);
    //muteButton.setBounds(bypassButton.getRight() + margin, buttonY, buttonSize, buttonSize);
    //soloButton.setBounds(muteButton.getRight() + margin, buttonY, buttonSize, buttonSize);

    const int sliderHeight = getHeight() - 113;
    bandGainSlider.setBounds(sliderX, sliderY, sliderWidth, sliderHeight);
    bandClipSlider.setBounds(bandGainSlider.getRight() + 5, sliderY, sliderWidth, sliderHeight);

    const int waveY = getHeight() - 46;
    m_clipWave.setBounds(waveX, waveY, waveW, waveH);


    m_lowMidSlider.setBounds(rotaryX, rotaryY, rotarySize, rotarySize);
    m_midHighSlider.setBounds(m_lowMidSlider.getRight() + margin, rotaryY, rotarySize, rotarySize);
}

void ClipperBandControls::changeListenerCallback(juce::ChangeBroadcaster* source)
{
    updateAttachments();
    resized();
    repaint();
}
