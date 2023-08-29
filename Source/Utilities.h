/*
  ==============================================================================

    Utilities.h
    Created: 28 Aug 2023 5:16:19pm
    Author:  Paxton Fleming

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

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