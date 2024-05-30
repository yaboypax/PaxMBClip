/*
  ==============================================================================

    LinearPhaseEQ.h
    Created: 28 May 2024 1:51:55pm
    Author:  Audio

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include <chowdsp_eq/chowdsp_eq.h>
#include <chowdsp_eq/EQ/chowdsp_LinearPhaseEQ.h>

namespace
{
    constexpr int kImpulseSize = 4096;
}


class LinearPhaseEQ
{
public:
    LinearPhaseEQ()
    {
        m_lowPass1.updatePrototypeEQParameters = [](auto& eq, auto& eqParams)
            { eq.setParameters(eqParams); };
    }
    void prepare(const juce::dsp::ProcessSpec& spec)
    {
        chowdsp::EQ::BasicEQParams<1> params;
        params.bands[0].params.bandFreqHz = 500.f;
        params.bands[0].params.bandQ = 1.f;
        params.bands[0].params.bandGainDB = 0.f;
        params.bands[0].params.bandType = 0;
        params.bands[0].params.bandOnOff = true;

        m_lowPass1.prepare(spec, params);
    }
    
    void process(juce::dsp::ProcessContextReplacing<float>& context)
    {
        m_lowPass1.process(context);
    }
    int getLatencySamples()
    {
        return m_lowPass1.getLatencySamples();
    }

private:
    chowdsp::EQ::LinearPhaseEQ <chowdsp::EQ::LinearPhasePrototypeEQ<float, chowdsp::EQ::BasicEQParams<1>, 1, chowdsp::EQ::EQBand<float, chowdsp::FirstOrderLPF<float>>>> m_lowPass1;
};

