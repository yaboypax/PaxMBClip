/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "ClipperBandControls.h"
#include "GlobalControls.h"
#include "SpectrumAnalyzer.h"
#include "ControlBar.h"
#include "SettingsComponent.h"
#include "LookAndFeel.h"
//===============================================================================

class PaxMBClipAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    PaxMBClipAudioProcessorEditor (PaxMBClipAudioProcessor& p);
    ~PaxMBClipAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    PaxMBClipAudioProcessor& m_processor;
    juce::SharedResourcePointer<ChompLookAndFeel> chompLAF;

    ControlBar controlBar {m_processor};
    GlobalControls globalControls {m_processor};
    ClipperBandControls bandControls {m_processor};
    SpectrumAnalyzer analyzer {m_processor};
    SettingsComponent settingsComponent{ m_processor };

    void toggleGlobalBypassState();
    void updateGlobalBypassButton();

    std::array<juce::AudioParameterBool*, 3> getBypassParams();


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PaxMBClipAudioProcessorEditor)
};
