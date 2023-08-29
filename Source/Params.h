/*
  ==============================================================================

    Params.h
    Created: 28 Aug 2023 4:58:30pm
    Author:  Paxton Fleming

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include <stdio.h>

namespace Params
{
    enum Names
    {
        Low_Mid_Crossover_Freq,
        Mid_High_Crossover_Freq,

        Low_Clip,
        Mid_Clip,
        High_Clip,

        Low_Gain,
        Mid_Gain,
        High_Gain,

        Bypassed_Low,
        Bypassed_Mid,
        Bypassed_High,

        Mute_Low,
        Mute_Mid,
        Mute_High,

        Solo_Low,
        Solo_Mid,
        Solo_High,

        Gain_In,
        Gain_Out
    };

    inline const std::map<Names, juce::String>& GetParams()
    {
        static std::map<Names, juce::String> params =
        {
            {Low_Mid_Crossover_Freq, "Low-Mid Crossover Freq"},
            {Mid_High_Crossover_Freq, "Mid-High Crossover Freq"},

            {Low_Clip, "Low Clip"},
            {Mid_Clip, "Mid Clip"},
            {High_Clip, "High Clip"},

            {Low_Gain, "Low Gain"},
            {Mid_Gain, "Mid Gain"},
            {High_Gain, "High Gain"},

            {Bypassed_Low, "Bypassed_Low"},
            {Bypassed_Mid, "Bypassed Mid"},
            {Bypassed_High, "Bypassed High"},

            {Mute_Low, "Mute Low"},
            {Mute_Mid, "Mute Mid"},
            {Mute_High,"Mute High"},

            {Solo_Low, "Solo Low"},
            {Solo_Mid, "Solo Mid"},
            {Solo_High, "Solo High"},

            {Gain_In, "Gain In"},
            {Gain_Out, "Gain Out"}
        };

        return params;
    }
}