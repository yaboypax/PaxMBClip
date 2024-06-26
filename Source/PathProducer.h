/*
  ==============================================================================

    PathProducer.h
    Created: 29 Aug 2023 10:17:31pm
    Author:  Paxton Fleming

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "FFTDataGenerator.h"
#include "AnalyzerPathGenerator.h"
#include "PluginProcessor.h"


struct PathProducer
{
    PathProducer(SingleChannelSampleFifo<PaxMBClipAudioProcessor::BlockType>& scsf) :
        monoChannelFifo(&scsf)
    {
        
        monoChannelFFTDataGenerator.changeOrder(FFTOrder::order2048);
        monoBuffer.setSize(1, monoChannelFFTDataGenerator.getFFTSize());
    }
    void process(juce::Rectangle<float> fftBounds, double sampleRate);
    juce::Path getPath() { return monoChannelFFTPath; }

    void updateNegativeInfinity(float nf) { negativeInfinity = nf; }
private:
    SingleChannelSampleFifo<PaxMBClipAudioProcessor::BlockType>* monoChannelFifo;

    juce::AudioBuffer<float> monoBuffer;
    std::vector<float> previousFFTBinMagnitudes;
    float attackRate = 0.1f;
    float decayRate = 0.01f;


    FFTDataGenerator<std::vector<float>> monoChannelFFTDataGenerator;

    AnalyzerPathGenerator analyzerPathGenerator;

    juce::Path monoChannelFFTPath;

    float negativeInfinity{ -48.f };
};