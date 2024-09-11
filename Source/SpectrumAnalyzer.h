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

struct CrossoverComponent : juce::Component , juce::MouseListener
{
    CrossoverComponent(PaxMBClipAudioProcessor& inProcessor, std::function<void()> callback)
    {
        m_processor = &inProcessor;
        setAlpha(0.5f);
        
        using namespace Params;
        const auto& params = GetParams();
        makeAttachment(m_lowCrossoverAttachment, m_processor->apvts, params, Names::Low_Mid_Crossover_Freq, m_lowCrossover);
        makeAttachment(m_highCrossoverAttachment, m_processor->apvts, params, Names::Mid_High_Crossover_Freq, m_highCrossover);

        auto chompLAF = juce::SharedResourcePointer<ChompLookAndFeel>();
        m_lowCrossover.setLookAndFeel(chompLAF);
        m_highCrossover.setLookAndFeel(chompLAF);

        m_lowCrossover.onValueChange = callback;
        m_highCrossover.onValueChange = callback;
        m_lowCrossover.onDragEnd = callback;
        m_highCrossover.onDragEnd = callback;

        addAndMakeVisible(m_lowCrossover);
        addAndMakeVisible(m_highCrossover);

        m_crossoverLabel.setText("CROSSOVERS", juce::NotificationType::dontSendNotification);
        m_crossoverLabel.setLookAndFeel(chompLAF);
        addAndMakeVisible(m_crossoverLabel);

    }

    void mouseEnter(const juce::MouseEvent& ev) override
    {
        setAlpha(0.9f);
        repaint();
    }

    void mouseExit(const juce::MouseEvent& ev) override
    {
        setAlpha(0.5f);
        repaint();
    }

    void paint(juce::Graphics& g) override
    {
        auto bounds = getLocalBounds();
        auto background = juce::Colours::black;

        g.setColour(background);
        g.fillRoundedRectangle(bounds.toFloat(), 24.f);
    }
    
    void resized() override
    {
        int size = 60;
        int margin = 14;
        m_lowCrossover.setBounds(margin, 0, size, size);
        m_highCrossover.setBounds(m_lowCrossover.getRight() + 5, 0, size, size);
        m_crossoverLabel.setBounds(margin +14,
            m_lowCrossover.getBottom() - 6, getWidth(), 24);
    }

    PaxMBClipAudioProcessor* m_processor;
    juce::SharedResourcePointer<ChompLookAndFeel> chompLAF;

    using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    RotarySlider m_lowCrossover, m_highCrossover;
    std::unique_ptr<SliderAttachment> m_lowCrossoverAttachment, m_highCrossoverAttachment;
    juce::Label m_crossoverLabel;
};

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

    juce::Rectangle<int> getHighlightArea();
    void timerCallback() override;
    void paint(juce::Graphics& g) override;

    void resized() override;
    void centerWaveformDisplay();
    void centerBandControls();  //resize band component and band buttons

    void mouseDown(const juce::MouseEvent& e) override;
    void mouseDrag(const juce::MouseEvent& e) override;
    void mouseUp(const juce::MouseEvent& e) override;
    void mouseWheelMove(const juce::MouseEvent& event, const juce::MouseWheelDetails& wheel) override;

    void buttonClicked(juce::Button* button) override;

    void scrollClip(float deltaY, const float scrollSpeed, juce::AudioParameterFloat* param);
    void scrollGain(float deltaY, const float scrollSpeed, juce::AudioParameterFloat* param);

    int mapFrequencyToX(const float frequency);
    int mapXToFrequency(const int x);


    void changeListenerCallback(juce::ChangeBroadcaster* source) override;

private:
    PaxMBClipAudioProcessor& m_processor;
    ClipperBandControls bandControls{ m_processor };
    juce::Label m_bandLabel;
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

    std::function<void()> m_crossoverCallback = [this]() {
        centerWaveformDisplay();
        centerBandControls();
        };
    CrossoverComponent m_crossoverComponent { m_processor, m_crossoverCallback };

    juce::Label m_frequencyDisplay;
    bool m_shouldDisplayWaveform = false;

    //RotarySlider m_lowMidSlider, m_midHighSlider;
    //std::unique_ptr<SliderAttachment> m_lowMidAttachment, m_midHighAttachment;


};

