/*
  ==============================================================================

    SpectrumAnalyzer.h
    Created: 29 Aug 2023 10:14:08pm
    Author:  Paxton Fleming

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "PathProducer.h"
#include "RotarySlider.h"
#include "LookAndFeel.h"
#include "ClipperBandControls.h"

class SpectrumAnalyzer : public juce::Component,
    public juce::AudioProcessorParameter::Listener,
    public juce::Timer, public juce::MouseListener, public juce::ChangeListener, public juce::Button::Listener
{
public:

    SpectrumAnalyzer(PaxMBClipAudioProcessor&);
    ~SpectrumAnalyzer();

    void layoutBandButtons();
    void setAttachments();
    void parameterValueChanged(int parameterIndex, float newValue) override;
    void parameterGestureChanged(int parameterIndex, bool gestureIsStarting) override { }

    void timerCallback() override;
    void paint(juce::Graphics& g) override;
    void resized() override;

    void mouseDown(const juce::MouseEvent& e) override;
    void mouseDrag(const juce::MouseEvent& e) override;
    void mouseUp(const juce::MouseEvent& e) override;
    void mouseWheelMove(const juce::MouseEvent& event, const juce::MouseWheelDetails& wheel) override;

    void buttonClicked(juce::Button* button) override;

    void scrollClip(float deltaY, const float scrollSpeed, juce::AudioParameterFloat* param);
    void scrollGain(float deltaY, const float scrollSpeed, juce::AudioParameterFloat* param);

    int mapFrequencyToX(const float frequency);
    int mapXToFrequency(const int x);

    void createCrossoverSliders(const juce::Point<int> point);
    void deleteCrossoverSliders();
    void changeListenerCallback(juce::ChangeBroadcaster* source) override;

private:
    PaxMBClipAudioProcessor& m_processor;
    ClipperBandControls bandControls{ m_processor };
    juce::SharedResourcePointer<ChompLookAndFeel> chompLAF;
    bool shouldShowFFTAnalysis = true;

    juce::Atomic<bool> parametersChanged{ false };

    void drawBackgroundGrid(juce::Graphics& g);
    void drawTextLabels(juce::Graphics& g);

    std::vector<float> getFrequencies();
    std::vector<float> getGains();
    std::vector<float> getXs(const std::vector<float>& freqs, float left, float width);

    juce::Rectangle<int> getRenderArea();
    juce::Rectangle<int> getAnalysisArea();

    PathProducer inPathProducer, outPathProducer;

    void drawCrossovers(juce::Graphics& g, juce::Rectangle<int> bounds);
    juce::AudioParameterFloat* m_lowCrossoverParam{ nullptr };
    juce::AudioParameterFloat* m_highCrossoverParam{ nullptr };
    
    int m_lowMidX = 200;
    int m_midHighX = 2000;

    using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    std::unique_ptr<SliderAttachment> lowCrossoverAttachment, highCrossoverAttachment;

    
    juce::ToggleButton m_soloLowButton, m_muteLowButton;
    juce::ToggleButton m_soloMidButton, m_muteMidButton;
    juce::ToggleButton m_soloHighButton, m_muteHighButton;

    using ButtonAttachment = juce::AudioProcessorValueTreeState::ButtonAttachment;
    std::unique_ptr<ButtonAttachment> m_soloLowAttachment, m_muteLowAttachment;
    std::unique_ptr<ButtonAttachment> m_soloMidAttachment, m_muteMidAttachment;
    std::unique_ptr<ButtonAttachment> m_soloHighAttachment, m_muteHighAttachment;

    bool m_lowMidDragging = false;
    bool m_midHighDragging = false;

    juce::AudioParameterFloat* m_lowClipParam{ nullptr };
    juce::AudioParameterFloat* m_midClipParam{ nullptr };
    juce::AudioParameterFloat* m_highClipParam{ nullptr };

    juce::AudioParameterFloat* m_lowGainParam{ nullptr };
    juce::AudioParameterFloat* m_midGainParam{ nullptr };
    juce::AudioParameterFloat* m_highGainParam{ nullptr };

    std::vector<std::shared_ptr<RotarySlider>> m_crossoverSliders;
    std::atomic<bool> m_showCrossoverSliders;

    juce::Label m_frequencyDisplay;
    bool m_shouldDisplayWaveform = false;

};

