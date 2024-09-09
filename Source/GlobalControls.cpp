/*
  ==============================================================================

    GlobalControls.cpp
    Created: 29 Aug 2023 8:20:08pm
    Author:  flemi

  ==============================================================================
*/

#include "GlobalControls.h"

namespace
{
    constexpr int sliderX = 26;
    constexpr int sliderY = 53;
    constexpr int sliderWidth = 48;

    constexpr int clipX = 26;
    constexpr int clipY = 18;
    constexpr int clipW = 100;
    constexpr int clipH = 24;

    constexpr int meterX = 25;
    constexpr int meterY = 53;
    constexpr int meterH = 115;

    constexpr int txMargin = 6;
    constexpr int tyMargin = 0;
    constexpr int textSize = 40;

}



GlobalControls::GlobalControls(PaxMBClipAudioProcessor& inProcessor)
{
    m_processor = &inProcessor;
    setupLevelMeters();

    using namespace Params;
    const auto& params = GetParams();

    makeAttachment(inGainSliderAttachment, m_processor->apvts, params, Names::Gain_In, inGainSlider);
    makeAttachment(outGainSliderAttachment, m_processor->apvts, params, Names::Gain_Out, outGainSlider);

    auto chompLAF = juce::SharedResourcePointer<ChompLookAndFeel>();
    inGainSlider.setSliderStyle(juce::Slider::LinearVertical);
    inGainSlider.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
    inGainSlider.setRange(-24.0, 24.0, 0.1);
    inGainSlider.setLookAndFeel(chompLAF);

    outGainSlider.setSliderStyle(juce::Slider::LinearVertical);
    outGainSlider.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
    outGainSlider.setRange(-24.0, 24.0, 0.1);
    outGainSlider.setLookAndFeel(chompLAF);

    addAndMakeVisible(inGainSlider);
    addAndMakeVisible(outGainSlider);

    masterClipButton.setButtonText("Master Clip");
    masterClipButton.setLookAndFeel(chompLAF);
    addAndMakeVisible(masterClipButton);

    masterClipButton.setClickingTogglesState(true);
    makeAttachment(masterClipAttachment, m_processor->apvts, params, Names::Master_Clip, masterClipButton);
}

GlobalControls::~GlobalControls()
{
    inGainSlider.setLookAndFeel(nullptr);
    outGainSlider.setLookAndFeel(nullptr);
}

void GlobalControls::setupLevelMeters()
{
    levelMeterInLAF->setMeterColour(LevelMeterLookAndFeel::lmMeterGradientLowColour, juce::Colours::green);
    levelMeterInLAF->setMeterColour(LevelMeterLookAndFeel::lmMeterMaxOverColour, juce::Colours::red);
    levelMeterInLAF->setMeterColour(LevelMeterLookAndFeel::lmBackgroundColour, juce::Colours::transparentWhite);
    levelMeterInLAF->setMeterColour(LevelMeterLookAndFeel::lmTicksColour, juce::Colours::transparentBlack);
    levelMeterInLAF->setMeterColour(LevelMeterLookAndFeel::lmOutlineColour, juce::Colours::transparentWhite);

    levelMeterOutLAF->setMeterColour(LevelMeterLookAndFeel::lmMeterGradientLowColour, juce::Colours::green);
    levelMeterOutLAF->setMeterColour(LevelMeterLookAndFeel::lmMeterMaxOverColour, juce::Colours::red);
    levelMeterOutLAF->setMeterColour(LevelMeterLookAndFeel::lmBackgroundColour, juce::Colours::transparentWhite);
    levelMeterOutLAF->setMeterColour(LevelMeterLookAndFeel::lmTicksColour, juce::Colours::transparentBlack);
    levelMeterOutLAF->setMeterColour(LevelMeterLookAndFeel::lmOutlineColour, juce::Colours::transparentWhite);

    m_inputMeter.setMeterSource(&m_processor->getMeterSourceIn());
    m_outputMeter.setMeterSource(&m_processor->getMeterSourceOut());

    addAndMakeVisible(m_inputMeter);
    addAndMakeVisible(m_outputMeter);
}


void GlobalControls::paint(juce::Graphics& g)
{
    g.setColour(juce::Colours::white);
    g.setFont(Chomp::Overhead.withHeight(42.f));

    g.drawText("IN", inGainSlider.getX() + txMargin, inGainSlider.getBottom() + tyMargin, textSize, textSize, juce::Justification::centred);
    g.drawText("OUT", outGainSlider.getX() + txMargin, outGainSlider.getBottom() + tyMargin, textSize, textSize, juce::Justification::centred);
}

void GlobalControls::resized()
{
    m_inputMeter.setBounds(meterX, meterY, 50, getHeight() - meterH);
    m_outputMeter.setBounds(meterX + 53, meterY, 50, getHeight() - meterH);

    const int sliderHeight = getHeight() - 113;
    inGainSlider.setBounds(sliderX, sliderY, sliderWidth, sliderHeight);
    outGainSlider.setBounds(inGainSlider.getRight() + 5, sliderY, sliderWidth, sliderHeight);

    masterClipButton.setBounds(clipX, clipY, clipW, clipH);
       
}