/*
  ==============================================================================

    ClipperBandControls.h
    Created: 29 Aug 2023 8:06:37pm
    Author:  flemi

  ==============================================================================
*/

#pragma once
#include "JuceHeader.h"
#include "PluginProcessor.h"

#include "RotarySlider.h"
#include "LookAndFeel.h"
#include "Params.h"

class ClipperBandControls : public juce::Component, public juce::ChangeListener
{
public:

    ClipperBandControls(PaxMBClipAudioProcessor& inProcessor);
    ~ClipperBandControls() override;

    void paint(juce::Graphics& g) override;
    void resized() override;

    void layoutSliders();
    void layoutButtons();
    void layoutWaveSelector();

    void updateAttachments();
    void changeListenerCallback(juce::ChangeBroadcaster* source) override;

private:
    PaxMBClipAudioProcessor* m_processor;
    BandFocus m_focus = BandFocus::unfocused;

    using Attachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    juce::Slider bandGainSlider, bandClipSlider;
    std::unique_ptr<Attachment> bandGainSliderAttachment, bandClipSliderAttachment;

    using BtnAttachment = juce::AudioProcessorValueTreeState::ButtonAttachment;
    juce::ToggleButton bypassButton, soloButton, muteButton;
    std::unique_ptr<BtnAttachment> bypassButtonAttachment, soloButtonAttachment, muteButtonAttachment;

    using ComboAttachment = juce::AudioProcessorValueTreeState::ComboBoxAttachment;
    juce::ComboBox waveSelection;
    std::unique_ptr<ComboAttachment> waveSelectionAttachment;

    juce::StringArray waveTypeNames = {
    "Hard Clip",
    "Quintic",
    "Cubic",
    "Hyperbolic Tan",
    "Algebraic",
    "Arctangent"
    };
};