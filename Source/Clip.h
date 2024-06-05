/*
  ==============================================================================

    Clip.h
    Created: 28 Aug 2023 5:04:32pm
    Author:  Paxton Fleming

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "Utilities.h"

enum class WaveType
{
    hard = 0,
    soft,
    smooth
};

class Clipper
{
public:
  
    juce::AudioParameterFloat* gain{ nullptr };
    juce::AudioParameterFloat* clip{ nullptr };
    juce::AudioParameterBool* bypassed{ nullptr };
    juce::AudioParameterBool* mute{ nullptr };
    juce::AudioParameterBool* solo{ nullptr };
    juce::AudioParameterInt* waveType{ nullptr };

    void prepare(const juce::dsp::ProcessSpec& spec);

    void updateClipperSettings();

    void process(juce::AudioBuffer<float>& buffer);

    float getRMSOutputLevelDb() const { return rmsOutputLevelDb; }
    float getRMSInputLevelDb() const { return rmsInputLevelDb; }

    void masterClip(juce::AudioBuffer<float>* buffer);



private:

    float m_softness = 0.0;
    void clipSamples(juce::AudioBuffer<float>* buffer, WaveType inWaveType);
    float hard(float& s);
    float quintic(float& s);
    float arctangent(float& s, float& soft);


    std::atomic<float> rmsInputLevelDb{ PaxMBClip::NEG_INFINITY };
    std::atomic<float> rmsOutputLevelDb{ PaxMBClip::NEG_INFINITY };

    juce::dsp::Gain<float> bandGain;

    WaveType m_waveType = WaveType::soft;


    template<typename T>
    float computeRMSLevel(const T& buffer)
    {
        int numChannels = static_cast<int>(buffer.getNumChannels());
        int numSamples = static_cast<int>(buffer.getNumSamples());
        auto rms = 0.f;
        for (int chan = 0; chan < numChannels; ++chan)
        {
            rms += buffer.getRMSLevel(chan, 0, numSamples);
        }

        rms /= static_cast<float>(numChannels);
        return rms;
    }

    template<typename T, typename U>
    void applyGain(T& buffer, U& dsp)
    {
        auto block = juce::dsp::AudioBlock<float>(buffer);
        auto ctx = juce::dsp::ProcessContextReplacing<float>(block);
        dsp.process(ctx);
    }

    template <typename T> int sgn(T val) {
        return (T(0) < val) - (val < T(0));
    }

};
