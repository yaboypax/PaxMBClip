/*
  ==============================================================================

    SettingsComponent.cpp
    Created: 29 Feb 2024 3:29:34pm
    Author:  Audio

  ==============================================================================
*/

#include "SettingsComponent.h"

SettingsComponent::SettingsComponent(PaxMBClipAudioProcessor& inProcessor)
{
    m_processor = &inProcessor;

    using namespace Params;
    const auto& params = GetParams();
    auto chompLAF = juce::SharedResourcePointer<ChompLookAndFeel>();


    for (int i = 0; i <= 5; ++i)
    {
        m_oversamplingSelection.addItem(getOversamplingPower(i), i + 1);
    }
    
    m_oversamplingSelection.setLookAndFeel(chompLAF);
    makeAttachment(m_oversamplingAttachment, m_processor->apvts, params, Names::Oversample, m_oversamplingSelection);
    addAndMakeVisible(m_oversamplingSelection);

    m_oversamplingLabel.setText("Oversampling", juce::NotificationType::dontSendNotification);
    m_oversamplingLabel.attachToComponent(&m_oversamplingSelection, true);
    addAndMakeVisible(m_oversamplingLabel);

    makeAttachment(m_masterWaveAttachment, m_processor->apvts, params, Names::Master_Wave, m_masterWave);
    addAndMakeVisible(m_masterWave);

    m_masterWaveLabel.setText("Master Clip Wave", juce::NotificationType::dontSendNotification);
    m_masterWaveLabel.attachToComponent(&m_masterWave, true);
    addAndMakeVisible(m_masterWaveLabel);

    m_analyzerToggle.addItem("Off", 1);
    m_analyzerToggle.addItem("On", 2);
    m_analyzerToggle.setSelectedId(2, true);
    m_analyzerToggle.onChange = [this]()
        {
            m_processor->toggleAnalyzer(static_cast<bool>(m_analyzerToggle.getSelectedId() - 1));
        };
    m_analyzerToggle.setLookAndFeel(chompLAF);
    addAndMakeVisible(m_analyzerToggle);

    m_analyzerLabel.setText("Analyzer On / Off", juce::NotificationType::dontSendNotification);
    m_analyzerLabel.attachToComponent(&m_analyzerToggle, true);
    addAndMakeVisible(m_analyzerLabel);

    m_phaseSetting.addItem("linear", 1);
    m_phaseSetting.addItem("minimum", 2);
    m_phaseSetting.setSelectedId(2, true);
    makeAttachment(m_phaseSettingAttachment, m_processor->apvts, params, Names::Phase_Response, m_phaseSetting);
    m_phaseSetting.setLookAndFeel(chompLAF);
    addAndMakeVisible(m_phaseSetting);

    m_phaseSettingLabel.setText("Phase Response", juce::NotificationType::dontSendNotification);
    m_phaseSettingLabel.attachToComponent(&m_phaseSetting, true);
    addAndMakeVisible(m_phaseSettingLabel);

}

void SettingsComponent::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::black.withAlpha(0.8f));
    drawModuleBackground(g, getLocalBounds());

    g.setColour(juce::Colours::white);
    g.setFont(Chomp::Overhead.withHeight(36.f));
    //auto textBounds = getLocalBounds().removeFromBottom(getHeight() * 0.8f);
    g.drawText("Settings", getLocalBounds(), juce::Justification::centredTop);
}

void SettingsComponent::resized()
{
    m_oversamplingSelection.setBounds(getWidth()/2, static_cast<int>(getHeight() * 0.2f), 120, 24);
    m_masterWave.setBounds(getWidth() / 2, m_oversamplingSelection.getBottom() + 5, 120, 24);
    m_analyzerToggle.setBounds(getWidth() / 2, m_masterWave.getBottom() + 5, 120, 24);
    m_phaseSetting.setBounds(getWidth() / 2, m_analyzerToggle.getBottom() + 5, 120, 24);
}