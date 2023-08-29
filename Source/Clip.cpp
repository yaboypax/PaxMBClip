/*
  ==============================================================================

    Clip.cpp
    Created: 28 Aug 2023 5:04:32pm
    Author:  Paxton Fleming

  ==============================================================================
*/

#include "Clip.h"

void Clipper::prepare(const juce::dsp::ProcessSpec& spec)
{
    clipper.prepare(spec);
    bandGain.prepare(spec);
}

void Clipper::updateClipperSettings()
{
    bandGain.setGainDecibels(gain->get());
    //clipper.setClip(release->get());
}

void Clipper::process(juce::AudioBuffer<float>& buffer)
{
    auto preRMS = computeRMSLevel(buffer);
    auto block = juce::dsp::AudioBlock<float>(buffer);
    auto context = juce::dsp::ProcessContextReplacing<float>(block);

    context.isBypassed = bypassed->get();

    applyGain(buffer, bandGain);

    //clipper.process(context);


    auto postRMS = computeRMSLevel(buffer);

    auto convertToDb = [](auto input)
    {
        return juce::Decibels::gainToDecibels(input);
    };

    rmsInputLevelDb.store(convertToDb(preRMS));
    rmsOutputLevelDb.store(convertToDb(postRMS));
}