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
        leftChannelFifo(&scsf)
    {
        leftChannelFFTDataGenerator.changeOrder(FFTOrder::order2048);
        monoBuffer.setSize(1, leftChannelFFTDataGenerator.getFFTSize());
    }
    void process(juce::Rectangle<float> fftBounds, double sampleRate);
    juce::Path getPath() { return leftChannelFFTPath; }

    void updateNegativeInfinity(float nf) { negativeInfinity = nf; }
private:
    SingleChannelSampleFifo<PaxMBClipAudioProcessor::BlockType>* leftChannelFifo;

    juce::AudioBuffer<float> monoBuffer;

    FFTDataGenerator<std::vector<float>> leftChannelFFTDataGenerator;

    AnalyzerPathGenerator<juce::Path> pathProducer;

    juce::Path leftChannelFFTPath;

    float negativeInfinity{ -48.f };
};