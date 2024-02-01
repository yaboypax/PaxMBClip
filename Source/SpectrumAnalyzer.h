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

class SpectrumAnalyzer : public juce::Component,
    public juce::AudioProcessorParameter::Listener,
    public juce::Timer, public juce::MouseListener
{
public:

    SpectrumAnalyzer(PaxMBClipAudioProcessor&);
    ~SpectrumAnalyzer();

    void parameterValueChanged(int parameterIndex, float newValue) override;
    void parameterGestureChanged(int parameterIndex, bool gestureIsStarting) override { }

    void timerCallback() override;
    void paint(juce::Graphics& g) override;
    void resized() override;

    void mouseDown(const juce::MouseEvent& e) override;
    void mouseDrag(const juce::MouseEvent& e) override;
    void mouseUp(const juce::MouseEvent& e) override;


    void toggleAnalysisEnablement(bool enabled)
    {
        shouldShowFFTAnalysis = enabled;
    }

private:
    PaxMBClipAudioProcessor& m_processor;

    bool shouldShowFFTAnalysis = true;

    juce::Atomic<bool> parametersChanged{ false };

    void drawBackgroundGrid(juce::Graphics& g);
    void drawTextLabels(juce::Graphics& g);

    std::vector<float> getFrequencies();
    std::vector<float> getGains();
    std::vector<float> getXs(const std::vector<float>& freqs, float left, float width);

    juce::Rectangle<int> getRenderArea();
    juce::Rectangle<int> getAnalysisArea();

    PathProducer leftPathProducer, rightPathProducer;

    void drawCrossovers(juce::Graphics& g, juce::Rectangle<int> bounds);
    juce::AudioParameterFloat* m_lowMidXoverParam{ nullptr };
    juce::AudioParameterFloat* m_midHighXoverParam{ nullptr };
    
    int m_lowMidX, m_midHighX;

    bool m_lowMidDragging = false;
    bool m_midHighDragging = false;

    juce::AudioParameterFloat* m_lowClipParam{ nullptr };
    juce::AudioParameterFloat* m_midClipParam{ nullptr };
    juce::AudioParameterFloat* m_highClipParam{ nullptr };


};

