/*
  ==============================================================================

    Clip.cpp
    Created: 28 Aug 2023 5:04:32pm
    Author:  Paxton Fleming

  ==============================================================================
*/

#include "Clip.h"

template <typename T> int sgn(T val) {
	return (T(0) < val) - (val < T(0));
}

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
	//clipSamples(&buffer, buffer.getNumChannels());
    //clipper.process(context);


    auto postRMS = computeRMSLevel(buffer);

    auto convertToDb = [](auto input)
    {
        return juce::Decibels::gainToDecibels(input);
    };

    rmsInputLevelDb.store(convertToDb(preRMS));
    rmsOutputLevelDb.store(convertToDb(postRMS));
}

void Clipper::clipSamples(juce::AudioBuffer<float>* buffer, int numchans)
{
	for (int j = 0; j < numchans; j++)
	{
		float* bufferData = buffer->getWritePointer(j);

		for (int i = 0; i < buffer->getNumSamples(); i++)
		{
			float newval = 0.0f;
			if (*waveType == 0)
				newval = hardclip(bufferData[i]);
			else if (*waveType == 1)
				newval = quintic(bufferData[i]);
			else if (*waveType == 2)
				newval = cubicBasic(bufferData[i]);
			else if (*waveType == 3)
				newval = tanclip(bufferData[i], m_softness);
			else if (*waveType == 4)
				newval = algclip(bufferData[i], m_softness);
			else if (*waveType == 5)
				newval = arcClip(bufferData[i], m_softness);
			bufferData[i] = newval;
		}
	}
}

float Clipper::sinclip(float& s)
{
	if (fabs(s) < M_PI)
	{
		return sin(s);
	}
	else
	{
		return sgn(s) * 1.0f;
	}
}

float Clipper::logiclip(float& s)
{
	return 2.0f / (1.0f + exp(-2.0f * s)) - 1.0f;
}

float Clipper::hardclip(float& s)
{
	return sgn(s) * fmin(fabs(s), 1.0f);
}

float Clipper::tanclip(float& s, float& soft)
{
	return tanh((1.0f - 0.5f * soft) * s);
}

float Clipper::quintic(float& s)
{
	if (fabs(s) < 1.25f)
		return s - (256.0f / 3125.0f) * powf(s, 5.0f);
	else
		return sgn(s) * 1.0f;
}

float Clipper::cubicBasic(float& s)
{
	if (fabs(s) < 1.5f)
		return s - (4.0f / 27.0f) * powf(s, 3.0f);
	else
		return sgn(s) * 1.0f;
}

float Clipper::algclip(float& s, float soft)
{
	return s / sqrtf((1.0f + 2.0f * soft + powf(s, 2.0f)));
}

float Clipper::arcClip(float& s, float& soft)
{
	return (2.0f / M_PI) * atan((1.6f - soft * 0.6f) * s);
}
