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
    //clipper.setClip(release->get());
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
				newval = hardclip(bufferData[i]);
				break;

			case WaveType::quintic:
				newval = quintic(bufferData[i]);
				break;

			case WaveType::cubic:
				newval = cubicBasic(bufferData[i]);
				break;

			case WaveType::tan:
				newval = tanclip(bufferData[i], m_softness);
				break;

			case WaveType::alg:
				newval = algclip(bufferData[i], m_softness);
				break;

			case WaveType::arc:
				newval = arcClip(bufferData[i], m_softness);
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
