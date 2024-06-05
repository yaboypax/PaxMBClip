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
    bandGain.prepare(spec);
}

void Clipper::updateClipperSettings()
{
    bandGain.setGainDecibels(gain->get());
}

void Clipper::process(juce::AudioBuffer<float>& buffer)
{
	bool isBypassed = bypassed->get();
	if (isBypassed)
		return;

    auto preRMS = computeRMSLevel(buffer);
    applyGain(buffer, bandGain);

	float clipCeiling = juce::Decibels::decibelsToGain(clip->get());

	buffer.applyGain(1 / clipCeiling);

	if (waveType)
		m_waveType = static_cast<WaveType>(waveType->get());

	clipSamples(&buffer, m_waveType);

	buffer.applyGain(clipCeiling);
	


    auto postRMS = computeRMSLevel(buffer);
    rmsInputLevelDb.store(juce::Decibels::gainToDecibels(preRMS));
    rmsOutputLevelDb.store(juce::Decibels::gainToDecibels(postRMS));
}

void Clipper::clipSamples(juce::AudioBuffer<float>* buffer, WaveType inWaveType)
{
	for (int j = 0; j < buffer->getNumChannels(); j++)
	{
		float* bufferData = buffer->getWritePointer(j);

		for (int i = 0; i < buffer->getNumSamples(); i++)
		{
			float newval = 0.0f;

			switch (inWaveType)
			{
			case WaveType::hard:
				newval = hard(bufferData[i]);
				break;

            case WaveType::soft:
				newval = quintic(bufferData[i]);
				break;

			case WaveType::smooth:
				newval = arctangent(bufferData[i], m_softness);
				break;
			}

			bufferData[i] = newval;
		}
	}
}

void Clipper::masterClip(juce::AudioBuffer<float>* buffer)
{
	if (waveType)
		m_waveType = static_cast<WaveType>(waveType->get());

	clipSamples(buffer, m_waveType);
}

float Clipper::hard(float& s)
{
	return sgn(s) * fmin(fabs(s), 1.0f);
}

float Clipper::quintic(float& s)
{
	if (fabs(s) < 1.25f)
		return s - (256.0f / 3125.0f) * powf(s, 5.0f);
	else
		return sgn(s) * 1.0f;
}

float Clipper::arctangent(float& s, float& soft)
{
	return (2.0f / M_PI) * atan((1.6f - soft * 0.6f) * s);
}
