/*
  ==============================================================================

    SettingsComponent.h
    Created: 29 Feb 2024 3:29:34pm
    Author:  Audio

  ==============================================================================
*/
#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "LookAndFeel.h"
#include "Params.h"
#include "Utilities.h"

class SettingsComponent : public juce::Component
{
public:
    SettingsComponent(PaxMBClipAudioProcessor& inProcessor);
    void paint(juce::Graphics& g) override;
    void resized() override;
private:
    PaxMBClipAudioProcessor* m_processor;

    using ComboAttachment = juce::AudioProcessorValueTreeState::ComboBoxAttachment;
    std::unique_ptr<ComboAttachment> m_oversamplingAttachment;
    juce::ComboBox m_oversamplingSelection;
    juce::Label m_oversamplingLabel;

    WaveSelector m_masterWave;
    std::unique_ptr<ComboAttachment> m_masterWaveAttachment;
    juce::Label m_masterWaveLabel;

    juce::ComboBox m_analyzerToggle;
    juce::Label m_analyzerLabel;
};