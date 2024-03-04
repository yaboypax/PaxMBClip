/*
  ==============================================================================

    ControlBar.h
    Created: 2 Sep 2023 7:39:03pm
    Author:  Paxton Fleming

  ==============================================================================
*/

#pragma once
#include "JuceHeader.h"
#include "PluginProcessor.h"
#include "LookAndFeel.h"
#include "Params.h"
#include "Utilities.h"

class ControlBar : public juce::Component
{
public:
    
    ControlBar(PaxMBClipAudioProcessor& inProcessor);

    void paint(juce::Graphics& g) override;
    void resized() override;

    PowerButton globalBypassButton;
    SettingsButton settingsButton;

private:
    PaxMBClipAudioProcessor* m_processor;
    juce::DrawableImage icon = juce::DrawableImage(juce::ImageCache::getFromMemory(BinaryData::chomp_icon_png, BinaryData::chomp_icon_pngSize));
};