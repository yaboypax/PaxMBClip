/*
  ==============================================================================

    AnalyzerPathGenerator.h
    Created: 29 Aug 2023 10:16:33pm
    Author:  Paxton Fleming

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

struct AnalyzerPathGenerator
{
    /*
     converts 'renderData[]' into a juce::Path
     */
    void generatePath(const std::vector<float>& renderData,
        juce::Rectangle<float> fftBounds,
        int fftSize,
        float binWidth,
        float negativeInfinity)
    {
        auto top = fftBounds.getY();
        auto bottom = fftBounds.getBottom();
        auto width = fftBounds.getWidth();

        int numBins = (int)fftSize / 2;

        juce::Path p;
        p.preallocateSpace(3 * (int)fftBounds.getWidth());

        auto map = [bottom, top, negativeInfinity](float v)
        {
            return juce::jmap(v,
                negativeInfinity, 0.f,
                bottom, top);
        };

        p.startNewSubPath(0, bottom);

        const int pathResolution = 1;
        for (int binNum = 1; binNum < numBins; binNum += pathResolution)
        {
            auto y = map(renderData[binNum]);

            if (!std::isnan(y) && !std::isinf(y))
            {
                auto binFreq = binNum * binWidth;
                auto normalizedBinX = juce::mapFromLog10(binFreq, PaxMBClip::MIN_FREQUENCY, PaxMBClip::MAX_FREQUENCY);
                float x = std::floor(normalizedBinX * width);
                p.lineTo(x, y);
            }
        }

        p.lineTo(width, bottom + 50);
        p.lineTo(0, bottom + 50);

        auto outputPath = p.juce::Path::createPathWithRoundedCorners(48.f);
        pathFifo.push(outputPath);
    }

    int getNumPathsAvailable() const
    {
        return pathFifo.getNumAvailableForReading();
    }

    bool getPath(juce::Path& path)
    {
        return pathFifo.pull(path);
    }
private:
    Fifo<juce::Path> pathFifo;
};
