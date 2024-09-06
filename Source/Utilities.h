/*
  ==============================================================================

    Utilities.h
    Created: 28 Aug 2023 5:16:19pm
    Author:  Paxton Fleming

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#define M_PI 3.14159265358979323846

static constexpr int MIN_HEIGHT = 500;
static constexpr int MIN_WIDTH = 800;


enum FFTOrder
{
    order2048 = 11,
    order4096 = 12,
    order8192 = 13
};


namespace PaxMBClip
{
    static constexpr float MIN_FREQUENCY = 5.f;
    static constexpr float MAX_FREQUENCY = 20000.f;

    static constexpr float NEG_INFINITY = -72.f;
    static constexpr float MAX_DB = 24.f;

    static constexpr float MIN_THRESHOLD = -60.f;



    enum FFTOrder
    {
        order2048 = 11,
        order4096 = 12,
        order8192 = 13
    };
}

template<typename Attachment, typename APVTS, typename Params, typename ParamName, typename SliderType>
void makeAttachment(std::unique_ptr<Attachment>& attachment, APVTS& apvts, const Params& params, const ParamName& name, SliderType& slider)
{
    attachment = std::make_unique<Attachment>(apvts, params.at(name), slider);
}

template<
    typename APVTS,
    typename Params,
    typename Name
>
juce::RangedAudioParameter& getParam(APVTS& apvts, const Params& params, const Name& name)
{
    auto param = apvts.getParameter(params.at(name));
    jassert(param != nullptr);

    return *param;
}

static inline float calcCutoff(float sampleRate)
{
    return (sampleRate / 2) * 0.98;
}

static inline juce::String getOversamplingPower(int ovr)
{
    return static_cast<juce::String>(pow(2, ovr)) + "x Oversampling";
}

static inline juce::AudioBuffer<float> sumBufferToMono(juce::AudioBuffer<float> buffer)
{
    buffer.addFrom(0, 0, buffer, 1, 0, buffer.getNumSamples());
    buffer.copyFrom(1, 0, buffer, 0, 0, buffer.getNumSamples());
    return buffer;
}