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

struct Clipper
{
    juce::AudioParameterFloat* gain{ nullptr };
    juce::AudioParameterFloat* clip{ nullptr };
    juce::AudioParameterBool* bypassed{ nullptr };
    juce::AudioParameterBool* mute{ nullptr };
    juce::AudioParameterBool* solo{ nullptr };

    juce::AudioParameterInt* waveType{ nullptr };
    float m_softness = 0.0;

    void prepare(const juce::dsp::ProcessSpec& spec);

    void updateClipperSettings();

    void process(juce::AudioBuffer<float>& buffer);

    float getRMSOutputLevelDb() const { return rmsOutputLevelDb; }
    float getRMSInputLevelDb() const { return rmsInputLevelDb; }

    void clipSamples(juce::AudioBuffer<float>* buffer, int numchans);

    float sinclip(float& s);
    float logiclip(float& s);
    float hardclip(float& s);
    float tanclip(float& s, float& soft);
    float quintic(float& s);
    float cubicBasic(float& s);
    float algclip(float& s, float soft);
    float arcClip(float& s, float& soft);


private:

    juce::dsp::WaveShaper<float> clipper;

    std::atomic<float> rmsInputLevelDb{ PaxMBClip::NEG_INFINITY };
    std::atomic<float> rmsOutputLevelDb{ PaxMBClip::NEG_INFINITY };

    juce::dsp::Gain<float> bandGain;
    //juce::AudioParameterFloat* inBandGainParam{ nullptr };
    //juce::AudioParameterFloat* outBandGainParam{ nullptr };

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
};