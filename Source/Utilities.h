/*
  ==============================================================================

    Utilities.h
    Created: 28 Aug 2023 5:16:19pm
    Author:  Paxton Fleming

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

enum FFTOrder
{
    order2048 = 11,
    order4096 = 12,
    order8192 = 13
};


namespace PaxMBClip
{
    static constexpr float MIN_FREQUENCY = 20.f;
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

void drawModuleBackground(juce::Graphics& g, juce::Rectangle<int> bounds);