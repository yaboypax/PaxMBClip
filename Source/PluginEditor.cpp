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

    setSize (600, 500);
    startTimerHz(40);
}

PaxMBClipAudioProcessorEditor::~PaxMBClipAudioProcessorEditor()
{
    stopTimer();
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

void PaxMBClipAudioProcessorEditor::toggleGlobalBypassState()
{
    bool shouldEnable = !controlBar.globalBypassButton.getToggleState();
    auto params = getBypassParams();
    auto bypassParamHelper = [](auto* param, bool shouldBeBypassed)
    {
        param->beginChangeGesture();
        param->setValueNotifyingHost(shouldBeBypassed ? 1.f : 0.f);
        param->endChangeGesture();
    };

    for (auto* param : params)
    {
        bypassParamHelper(param, !shouldEnable);
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

void PaxMBClipAudioProcessorEditor::timerCallback()
{
    updateGlobalBypassButton();
}

void PaxMBClipAudioProcessorEditor::updateGlobalBypassButton()
{
    auto params = getBypassParams();
    bool allBandsBypassed = std::all_of(params.begin(), params.end(), [](const auto& param)
     {  return param->get(); });
    
    controlBar.globalBypassButton.setToggleState(allBandsBypassed, juce::NotificationType::dontSendNotification);
}