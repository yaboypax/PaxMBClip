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

    void updateAttachments();
    void changeListenerCallback(juce::ChangeBroadcaster* source) override;
    bool m_isWaveformShowing = false;
private:
    PaxMBClipAudioProcessor* m_processor;
    BandFocus m_focus = BandFocus::unfocused;

    using Attachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    juce::Slider bandGainSlider, bandClipSlider;
    std::unique_ptr<Attachment> bandGainSliderAttachment, bandClipSliderAttachment;
    juce::Label m_clipGainLabel;

    RotarySlider m_lowMidSlider, m_midHighSlider;
    std::unique_ptr<Attachment> m_lowMidAttachment, m_midHighAttachment;
    juce::Label m_crossoverLabel;

    using ComboAttachment = juce::AudioProcessorValueTreeState::ComboBoxAttachment;
    WaveSelector m_clipWave;
    std::unique_ptr<ComboAttachment> m_clipWaveAttachment;
    juce::Label m_waveLabel;

};