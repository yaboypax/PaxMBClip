/*
  ==============================================================================

    PathProducer.cpp
    Created: 29 Aug 2023 10:17:31pm
    Author:  Paxton Fleming

  ==============================================================================
*/

#include "PathProducer.h"

void PathProducer::process(juce::Rectangle<float> fftBounds, double sampleRate)
{
    juce::AudioBuffer<float> tempIncomingBuffer;
    while (monoChannelFifo->getNumCompleteBuffersAvailable() > 0)
    {
        if (monoChannelFifo->getAudioBuffer(tempIncomingBuffer))
        {
            auto size = tempIncomingBuffer.getNumSamples();
            size = juce::jmin(size, monoBuffer.getNumSamples());

            auto writePointer = monoBuffer.getWritePointer(0, 0);
            auto readPointer = monoBuffer.getReadPointer(0, size);

            std::copy(readPointer, readPointer + (monoBuffer.getNumSamples() - size), writePointer);

            juce::FloatVectorOperations::copy(monoBuffer.getWritePointer(0, 0),
                monoBuffer.getReadPointer(0, size),
                monoBuffer.getNumSamples() - size);

            juce::FloatVectorOperations::copy(monoBuffer.getWritePointer(0, monoBuffer.getNumSamples() - size),
                tempIncomingBuffer.getReadPointer(0, 0),
                size);

            monoChannelFFTDataGenerator.produceFFTDataForRendering(monoBuffer, negativeInfinity);
        }
    }

    const auto fftSize = monoChannelFFTDataGenerator.getFFTSize();
    const auto binWidth = sampleRate / double(fftSize);

    while (monoChannelFFTDataGenerator.getNumAvailableFFTDataBlocks() > 0)
    {
        std::vector<float> fftData;
        if (monoChannelFFTDataGenerator.getFFTData(fftData))
        {
            if (previousFFTBinMagnitudes.empty()) {
                previousFFTBinMagnitudes.resize(fftData.size(), 0.0f);
            }

            for (size_t i = 0; i < fftData.size(); i++) {
                float rate = fftData[i] > previousFFTBinMagnitudes[i] ? attackRate : decayRate;
                fftData[i] = previousFFTBinMagnitudes[i] + rate * (fftData[i] - previousFFTBinMagnitudes[i]);
                previousFFTBinMagnitudes[i] = fftData[i];
            }

            analyzerPathGenerator.generatePath(fftData, fftBounds, fftSize, binWidth, negativeInfinity);
       
        }
    }

    while (analyzerPathGenerator.getNumPathsAvailable() > 0)
    {
        analyzerPathGenerator.getPath(monoChannelFFTPath);
    }
}