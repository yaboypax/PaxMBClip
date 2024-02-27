/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
PaxMBClipAudioProcessorEditor::PaxMBClipAudioProcessorEditor(PaxMBClipAudioProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p)

{
    setResizable(true, true);
    setResizeLimits(800, 500, 1600, 1000);

    controlBar.analyzerButton.onClick = [this]()
    {
        bool shouldBeOn = controlBar.analyzerButton.getToggleState();
        analyzer.toggleAnalysisEnablement(shouldBeOn);
    };

    controlBar.globalBypassButton.onClick = [this]()
    {
        toggleGlobalBypassState();
    };


    controlBar.setLookAndFeel(&lnf);
    bandControls.setLookAndFeel(&lnf);

    addAndMakeVisible(controlBar);
    addAndMakeVisible(analyzer);
    addAndMakeVisible(globalControls);
    addAndMakeVisible(bandControls);

    levelMeterInLAF->setMeterColour(LevelMeterLookAndFeel::lmMeterGradientLowColour, juce::Colours::green);
    levelMeterInLAF->setMeterColour(LevelMeterLookAndFeel::lmMeterMaxOverColour, juce::Colours::red);
    levelMeterInLAF->setMeterColour(LevelMeterLookAndFeel::lmBackgroundColour, juce::Colours::white);
    levelMeterInLAF->setMeterColour(LevelMeterLookAndFeel::lmTicksColour, juce::Colours::transparentBlack);
    levelMeterInLAF->setMeterColour(LevelMeterLookAndFeel::lmOutlineColour,juce::Colours::white);

    levelMeterOutLAF->setMeterColour(LevelMeterLookAndFeel::lmMeterGradientLowColour, juce::Colours::green);
    levelMeterOutLAF->setMeterColour(LevelMeterLookAndFeel::lmMeterMaxOverColour, juce::Colours::red);
    levelMeterOutLAF->setMeterColour(LevelMeterLookAndFeel::lmBackgroundColour, juce::Colours::white);
    levelMeterOutLAF->setMeterColour(LevelMeterLookAndFeel::lmTicksColour, juce::Colours::transparentBlack);
    levelMeterOutLAF->setMeterColour(LevelMeterLookAndFeel::lmOutlineColour, juce::Colours::white);

    inputMeter.setLookAndFeel(levelMeterInLAF);
    outputMeter.setLookAndFeel(levelMeterOutLAF);

    inputMeter.setMeterSource(&p.getMeterSourceIn());
    outputMeter.setMeterSource(&p.getMeterSourceOut());

    addAndMakeVisible(inputMeter);
    addAndMakeVisible(outputMeter);

}

PaxMBClipAudioProcessorEditor::~PaxMBClipAudioProcessorEditor()
{
    inputMeter.setLookAndFeel(nullptr);
    outputMeter.setLookAndFeel(nullptr);
}

//==============================================================================
void PaxMBClipAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll(juce::Colours::black);
    updateGlobalBypassButton();
}

void PaxMBClipAudioProcessorEditor::resized()
{
    auto bounds = getLocalBounds();

    controlBar.setBounds(bounds.removeFromTop(32));
    analyzer.setBounds(0, 32, getWidth()-300, getHeight() - 32);
    bandControls.setBounds(getWidth()-300, 32, 150, getHeight() - 32);
    globalControls.setBounds(getWidth() - 150, 32, 150, getHeight() - 32);

    inputMeter.setBounds(0, 32, 20, getHeight() - 32);
    outputMeter.setBounds(25, 32, 20, getHeight() - 32);
}
void PaxMBClipAudioProcessorEditor::setupLevelMeters()
{

}


void PaxMBClipAudioProcessorEditor::toggleGlobalBypassState()
{
    bool shouldEnable = !controlBar.globalBypassButton.getToggleState();
    auto params = getBypassParams();

    for (auto* param : params)
    {
        param->beginChangeGesture();
        param->setValueNotifyingHost(!shouldEnable ? 1.f : 0.f);
        param->endChangeGesture();
    }
}

std::array<juce::AudioParameterBool*, 3> PaxMBClipAudioProcessorEditor::getBypassParams()
{
    using namespace Params;
    const auto& params = Params::GetParams();
    auto& apvts = audioProcessor.apvts;

    auto boolHelper = [&apvts, &params](const auto& paramName)
    {
        auto param = dynamic_cast<juce::AudioParameterBool*>(apvts.getParameter(params.at(paramName)));
        jassert(param != nullptr);
        return param;
    };

    auto* lowBypassParam = boolHelper(Names::Bypassed_Low);
    auto* midBypassParam = boolHelper(Names::Bypassed_Mid);
    auto* highBypassParam = boolHelper(Names::Bypassed_High);

    return
    {
        lowBypassParam,
        midBypassParam,
        highBypassParam
    };
}

void PaxMBClipAudioProcessorEditor::updateGlobalBypassButton()
{
    auto params = getBypassParams();
    bool allBandsBypassed = std::all_of(params.begin(), params.end(), [](const auto& param)
     {  return param->get(); });
    
    controlBar.globalBypassButton.setToggleState(allBandsBypassed, juce::NotificationType::dontSendNotification);
}