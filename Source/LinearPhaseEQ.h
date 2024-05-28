/*
  ==============================================================================

    LinearPhaseEQ.h
    Created: 28 May 2024 1:51:55pm
    Author:  Audio

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include <chowdsp_plugin_base/chowdsp_plugin_base.h>
#include <chowdsp_eq/chowdsp_eq.h>
#include <chowdsp_eq/EQ/chowdsp_StandardEQParameters.h>

using EQParams = chowdsp::EQ::StandardEQParameters<3>;
static constexpr auto DecrampedMode = chowdsp::CoefficientCalculators::CoefficientCalculationMode::Decramped;
template <typename FloatType>
using EQBand = chowdsp::EQ::EQBand < FloatType,
    chowdsp::FirstOrderHPF<FloatType>,
    chowdsp::SecondOrderHPF<FloatType, DecrampedMode>,
    chowdsp::SVFHighpass<FloatType>,
    chowdsp::ButterworthFilter<3, chowdsp::ButterworthFilterType::Highpass, FloatType>,
    chowdsp::NthOrderFilter<FloatType, 4, chowdsp::StateVariableFilterType::Highpass>,
    chowdsp::ButterworthFilter<8, chowdsp::ButterworthFilterType::Highpass, FloatType>,
    chowdsp::ChebyshevIIFilter<8, chowdsp::ChebyshevFilterType::Highpass, 60, true, FloatType>,
    chowdsp::EllipticFilter<12, chowdsp::EllipticFilterType::Highpass, 60, chowdsp::Ratio<1, 10>, FloatType>,
    chowdsp::LowShelfFilter<FloatType, DecrampedMode>,
    chowdsp::SVFLowShelf<FloatType>,
    chowdsp::PeakingFilter<FloatType, DecrampedMode>,
    chowdsp::SVFBell<FloatType>,
    chowdsp::NotchFilter<FloatType, DecrampedMode>,
    chowdsp::SVFNotch<FloatType>,
    chowdsp::HighShelfFilter<FloatType, DecrampedMode>,
    chowdsp::SVFHighShelf<FloatType>,
    chowdsp::SecondOrderBPF<FloatType, DecrampedMode>,
    chowdsp::SVFBandpass<FloatType>,
    chowdsp::FirstOrderLPF<FloatType>,
    chowdsp::SecondOrderLPF<FloatType, DecrampedMode>,
    chowdsp::SVFLowpass<FloatType>,
    chowdsp::ButterworthFilter<3, chowdsp::ButterworthFilterType::Lowpass, FloatType>,
    chowdsp::NthOrderFilter<FloatType, 4, chowdsp::StateVariableFilterType::Lowpass>,
    chowdsp::ButterworthFilter<8, chowdsp::ButterworthFilterType::Lowpass, FloatType>,
    chowdsp::ChebyshevIIFilter<8, chowdsp::ChebyshevFilterType::Lowpass, 60, true, FloatType>,
    chowdsp::EllipticFilter<12, chowdsp::EllipticFilterType::Lowpass, 60, chowdsp::Ratio<1, 10>, FloatType >> ;
using PrototypeEQ = chowdsp::EQ::LinearPhasePrototypeEQ<float, EQParams::Params, EQParams::EQNumBands, EQBand<float>>;

#include <chowdsp_eq/EQ/chowdsp_LinearPhaseEQ.h>

namespace
{
    constexpr int kImpulseSize = 4096;
}


//struct Params : chowdsp::ParamHolder
//{
//    using BandParams = EQParams::EQBandParams;
//
//    inline static const juce::StringArray bandTypeChoices{
//        "1-Pole HPF",
//        "2-Pole HPF",
//        "2-Pole HPF SVF",
//        "3-Pole HPF",
//        "4-Pole HPF",
//        "8-Pole HPF",
//        "8-Pole Cheby. HPF",
//        "12-Pole Elliptic HPF",
//        "Low-Shelf",
//        "Low-Shelf SVF",
//        "Bell",
//        "Bell SVF",
//        "Notch",
//        "Notch SVF",
//        "High-Shelf",
//        "High-Shelf SVF",
//        "2-Pole BPF",
//        "2-Pole BPF SVF",
//        "1-Pole LPF",
//        "2-Pole LPF",
//        "2-Pole LPF SVF",
//        "3-Pole LPF",
//        "4-Pole LPF",
//        "8-Pole LPF",
//        "8-Pole Cheby. LPF",
//        "12-Pole Elliptic LPF",
//    };
//
//    Params()
//    {
//        add(eqParams, linPhaseMode);
//    }
//
//    EQParams eqParams{
//        {
//            BandParams { 0, "eq_band_0", "Band ", 100, bandTypeChoices, 10, 250.0f },
//            BandParams { 1, "eq_band_1", "Band ", 100, bandTypeChoices, 10, 1000.0f },
//            BandParams { 2, "eq_band_2", "Band ", 100, bandTypeChoices, 10, 4000.0f },
//        }
//    };
//
//    chowdsp::BoolParameter::Ptr linPhaseMode{
//        chowdsp::ParameterID { "linear_phase_mode", 100 },
//        "Linear Phase",
//        false
//    };
//};




chowdsp::EQ::LinearPhaseEQ <chowdsp::EQ::LinearPhasePrototypeEQ<float, chowdsp::EQ::BasicEQParams<1>, 1, chowdsp::EQ::EQBand<float, chowdsp::FirstOrderLPF<float>>>> m_lowPass1;
//chowdsp::EQ::LinearPhaseEQ <chowdsp::EQ::EQProcessor<float, 1, chowdsp::EQ::EQBand<float, chowdsp::FirstOrderHPF<float>>>> m_highPass1;
//chowdsp::EQ::LinearPhaseEQ <chowdsp::EQ::EQProcessor<float, 1, chowdsp::EQ::EQBand<float, chowdsp::FirstOrderLPF<float>>>> m_lowPass2;
//chowdsp::EQ::LinearPhaseEQ <chowdsp::EQ::EQProcessor<float, 1, chowdsp::EQ::EQBand<float, chowdsp::FirstOrderHPF<float>>>> m_highPass2;
