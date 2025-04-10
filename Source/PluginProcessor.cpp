/*
  =============================================================================
 __  __  ______       ______  ______  __  __       ______  ______  __  __
/\ \_\ \/\  __ \     /\  == \/\  __ \/\ \_\ \     /\  == \/\  __ \/\_\_\_\
\ \____ \ \  __ \    \ \  __<\ \ \/\ \ \____ \    \ \  _-/\ \  __ \/_/\_\/_
 \/\_____\ \_\ \_\    \ \_____\ \_____\/\_____\    \ \_\   \ \_\ \_\/\_\/\_\
  \/_____/\/_/\/_/     \/_____/\/_____/\/_____/     \/_/    \/_/\/_/\/_/\/_/

  Chomp - Multiband Clipper VST

  =============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
PaxMBClipAudioProcessor::PaxMBClipAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
    : AudioProcessor(BusesProperties()
#if ! JucePlugin_IsMidiEffect
#if ! JucePlugin_IsSynth
        .withInput("Input", juce::AudioChannelSet::stereo(), true)
#endif
        .withOutput("Output", juce::AudioChannelSet::stereo(), true)
#endif
    ),
#endif
    m_waveformDisplay(1)
{
    m_waveformDisplay.setRepaintRate(40);
    m_waveformDisplay.setBufferSize(256);
    m_waveformDisplay.setOpaque(false);
    m_waveformDisplay.setInterceptsMouseClicks(false, false);
    m_waveformDisplay.setColours(juce::Colours::transparentBlack, juce::Colours::white);

    initializeParameters();
    m_resizedBuffer = std::make_unique<juce::AudioBuffer<float>>(2, 0);
}

PaxMBClipAudioProcessor::~PaxMBClipAudioProcessor()
{
}

void PaxMBClipAudioProcessor::initializeParameters()
{
    using namespace Params;
    const auto& params = GetParams();

    lowBandClip.gain = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(Names::Low_Gain)));
    lowBandClip.clip = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(Names::Low_Clip)));
    lowBandClip.waveType = dynamic_cast<juce::AudioParameterInt*>(apvts.getParameter(params.at(Names::Low_Wave)));

    midBandClip.gain = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(Names::Mid_Gain)));
    midBandClip.clip = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(Names::Mid_Clip)));
    midBandClip.waveType = dynamic_cast<juce::AudioParameterInt*>(apvts.getParameter(params.at(Names::Mid_Wave)));

    highBandClip.gain = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(Names::High_Gain)));
    highBandClip.clip = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(Names::High_Clip)));
    highBandClip.waveType = dynamic_cast<juce::AudioParameterInt*>(apvts.getParameter(params.at(Names::High_Wave)));

    lowMidCrossover = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(Names::Low_Mid_Crossover_Freq)));
    midHighCrossover = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(Names::Mid_High_Crossover_Freq)));

    m_inputGainParam = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(Names::Gain_In)));
    m_outputGainParam = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(Names::Gain_Out)));
    m_oversampleParam = dynamic_cast<juce::AudioParameterInt*>(apvts.getParameter(params.at(Names::Oversample)));

    lowBandClip.bypassed = dynamic_cast<juce::AudioParameterBool*>(apvts.getParameter(params.at(Names::Bypassed_Low)));
    midBandClip.bypassed = dynamic_cast<juce::AudioParameterBool*>(apvts.getParameter(params.at(Names::Bypassed_Mid)));
    highBandClip.bypassed = dynamic_cast<juce::AudioParameterBool*>(apvts.getParameter(params.at(Names::Bypassed_High)));

    lowBandClip.mute = dynamic_cast<juce::AudioParameterBool*>(apvts.getParameter(params.at(Names::Mute_Low)));
    midBandClip.mute = dynamic_cast<juce::AudioParameterBool*>(apvts.getParameter(params.at(Names::Mute_Mid)));
    highBandClip.mute = dynamic_cast<juce::AudioParameterBool*>(apvts.getParameter(params.at(Names::Mute_High)));

    lowBandClip.solo = dynamic_cast<juce::AudioParameterBool*>(apvts.getParameter(params.at(Names::Solo_Low)));
    midBandClip.solo = dynamic_cast<juce::AudioParameterBool*>(apvts.getParameter(params.at(Names::Solo_Mid)));
    highBandClip.solo = dynamic_cast<juce::AudioParameterBool*>(apvts.getParameter(params.at(Names::Solo_High)));

    m_masterClipParam = dynamic_cast<juce::AudioParameterBool*>(apvts.getParameter(params.at(Names::Master_Clip)));
    m_masterClip.waveType = dynamic_cast<juce::AudioParameterInt*>(apvts.getParameter(params.at(Names::Master_Wave)));
    m_phaseResponseParam = dynamic_cast<juce::AudioParameterInt*>(apvts.getParameter(params.at(Names::Phase_Response)));


}

juce::AudioProcessorValueTreeState::ParameterLayout PaxMBClipAudioProcessor::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    using namespace Params;
    const auto& params = GetParams();

    layout.add(std::make_unique<juce::AudioParameterFloat>(params.at(Names::Low_Mid_Crossover_Freq), params.at(Names::Low_Mid_Crossover_Freq), juce::NormalisableRange<float>(PaxMBClip::MIN_FREQUENCY, PaxMBClip::MAX_FREQUENCY, 1.f, 0.2f), 200.f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(params.at(Names::Mid_High_Crossover_Freq), params.at(Names::Mid_High_Crossover_Freq), juce::NormalisableRange<float>(PaxMBClip::MIN_FREQUENCY, PaxMBClip::MAX_FREQUENCY, 1.f, 0.2f), 2000.f));

    auto clipLow = -48.0f;
    auto clipHigh = 0.0f;

    auto gainLow = -24.0f;
    auto gainHigh = 24.0f;

    layout.add(std::make_unique<juce::AudioParameterFloat>(params.at(Names::Low_Clip), params.at(Names::Low_Clip), clipLow, clipHigh, 0.f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(params.at(Names::Mid_Clip), params.at(Names::Mid_Clip), clipLow, clipHigh, 0.f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(params.at(Names::High_Clip), params.at(Names::High_Clip), clipLow, clipHigh, 0.f));

    layout.add(std::make_unique<juce::AudioParameterFloat>(params.at(Names::Low_Gain), params.at(Names::Low_Gain), gainLow, gainHigh, 0.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(params.at(Names::Mid_Gain), params.at(Names::Mid_Gain), gainLow, gainHigh, 0.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(params.at(Names::High_Gain), params.at(Names::High_Gain), gainLow, gainHigh, 0.0f));

    layout.add(std::make_unique<juce::AudioParameterInt>(params.at(Names::Low_Wave), params.at(Names::Low_Wave), 0, 2, 1));
    layout.add(std::make_unique<juce::AudioParameterInt>(params.at(Names::Mid_Wave), params.at(Names::Mid_Wave), 0, 2, 1));
    layout.add(std::make_unique<juce::AudioParameterInt>(params.at(Names::High_Wave), params.at(Names::High_Wave), 0, 2, 1));

    layout.add(std::make_unique<juce::AudioParameterBool>(params.at(Names::Bypassed_Low), params.at(Names::Bypassed_Low), false));
    layout.add(std::make_unique<juce::AudioParameterBool>(params.at(Names::Bypassed_Mid), params.at(Names::Bypassed_Mid), false));
    layout.add(std::make_unique<juce::AudioParameterBool>(params.at(Names::Bypassed_High), params.at(Names::Bypassed_High), false));

    layout.add(std::make_unique<juce::AudioParameterBool>(params.at(Names::Mute_Low), params.at(Names::Mute_Low), false));
    layout.add(std::make_unique<juce::AudioParameterBool>(params.at(Names::Mute_Mid), params.at(Names::Mute_Mid), false));
    layout.add(std::make_unique<juce::AudioParameterBool>(params.at(Names::Mute_High), params.at(Names::Mute_High), false));

    layout.add(std::make_unique<juce::AudioParameterBool>(params.at(Names::Solo_Low), params.at(Names::Solo_Low), false));
    layout.add(std::make_unique<juce::AudioParameterBool>(params.at(Names::Solo_Mid), params.at(Names::Solo_Mid), false));
    layout.add(std::make_unique<juce::AudioParameterBool>(params.at(Names::Solo_High), params.at(Names::Solo_High), false));

    layout.add(std::make_unique<juce::AudioParameterFloat>(params.at(Names::Gain_In), params.at(Names::Gain_In), gainLow, gainHigh, 0.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(params.at(Names::Gain_Out), params.at(Names::Gain_Out), gainLow, gainHigh, 0.0f));

    layout.add(std::make_unique<juce::AudioParameterInt>(params.at(Names::Oversample), params.at(Names::Oversample), 0, 5, 1));
    layout.add(std::make_unique<juce::AudioParameterBool>(params.at(Names::Master_Clip), params.at(Names::Master_Clip), false));
    layout.add(std::make_unique<juce::AudioParameterInt>(params.at(Names::Master_Wave), params.at(Names::Master_Wave), 0, 2, 0));
    layout.add(std::make_unique<juce::AudioParameterInt>(params.at(Names::Phase_Response), params.at(Names::Phase_Response), 0, 1, 1));

    return layout;
}

void PaxMBClipAudioProcessor::setCrossoverFilters()
{
    LP1.setType(juce::dsp::LinkwitzRileyFilterType::lowpass);
    HP1.setType(juce::dsp::LinkwitzRileyFilterType::highpass);

    AP2.setType(juce::dsp::LinkwitzRileyFilterType::allpass);

    LP2.setType(juce::dsp::LinkwitzRileyFilterType::lowpass);
    HP2.setType(juce::dsp::LinkwitzRileyFilterType::highpass);

}


//==============================================================================
const juce::String PaxMBClipAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool PaxMBClipAudioProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
    return true;
#else
    return false;
#endif
}

bool PaxMBClipAudioProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
    return true;
#else
    return false;
#endif
}

bool PaxMBClipAudioProcessor::isMidiEffect() const
{
#if JucePlugin_IsMidiEffect
    return true;
#else
    return false;
#endif
}

double PaxMBClipAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int PaxMBClipAudioProcessor::getNumPrograms()
{
    return 1;
}

int PaxMBClipAudioProcessor::getCurrentProgram()
{
    return 0;
}

void PaxMBClipAudioProcessor::setCurrentProgram(int index)
{
}

const juce::String PaxMBClipAudioProcessor::getProgramName(int index)
{
    return {};
}

void PaxMBClipAudioProcessor::changeProgramName(int index, const juce::String& newName)
{
}

//==============================================================================
void PaxMBClipAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    juce::dsp::ProcessSpec spec;
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = getTotalNumOutputChannels();
    spec.sampleRate = sampleRate;

    for (auto& clipper : clippers)
    {
        clipper.prepare(spec);
    }

    m_masterClip.prepare(spec);

    LP1.prepare(spec);
    HP1.prepare(spec);
    LP2.prepare(spec);
    HP2.prepare(spec);
    AP2.prepare(spec);

    setCrossoverFilters();
    m_LinearPhaseCrossover.reset();
    m_LinearPhaseCrossover.prepare(spec, kImpulseSize, 200.f, 2000.f);
        
    m_oversamplingFilter1.setup(kFOrder, sampleRate * m_oversample, calcCutoff(sampleRate));
    m_oversamplingFilter2.setup(kFOrder, sampleRate * m_oversample, calcCutoff(sampleRate));

    m_tiltFilter.prepare(spec);
    *m_tiltFilter.get<0>().state = *juce::dsp::IIR::Coefficients<float>::makeLowShelf(sampleRate, 1200, 1.f, juce::Decibels::decibelsToGain(-6.f));
    *m_tiltFilter.get<1>().state = *juce::dsp::IIR::Coefficients<float>::makeHighShelf(sampleRate, 1200, 1.f, juce::Decibels::decibelsToGain(6.f));

    for (auto& buffer : filterBuffers)
    {
        buffer.setSize(spec.numChannels, samplesPerBlock);
    }

    monoInFifo.prepare(samplesPerBlock);
    rightInFifo.prepare(samplesPerBlock);

    monoOutFifo.prepare(samplesPerBlock);
    rightOutFifo.prepare(samplesPerBlock);

    m_resizedBuffer->setSize(2, samplesPerBlock * kMaxOversample, false, true, false);
    m_resizedBuffer->clear();

    levelMeterSourceIn.resize(2, ceil((8192.0 / samplesPerBlock) * (sampleRate / 48000)));
    levelMeterSourceOut.resize(2, ceil((8192.0 / samplesPerBlock) * (sampleRate / 48000)));
}

void PaxMBClipAudioProcessor::releaseResources()
{
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool PaxMBClipAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
#if JucePlugin_IsMidiEffect
    juce::ignoreUnused(layouts);
    return true;
#else
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
        && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

#if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
#endif

    return true;
#endif
}
#endif
void PaxMBClipAudioProcessor::updateState()
{
    for (auto& clipper : clippers)
    {
        clipper.updateClipperSettings();
    }

    auto lowMidCutoffFreq = lowMidCrossover->get();
    LP1.setCutoffFrequency(lowMidCutoffFreq);
    HP1.setCutoffFrequency(lowMidCutoffFreq);

    auto midHighCutoffFreq = midHighCrossover->get();
    LP2.setCutoffFrequency(midHighCutoffFreq);
    HP2.setCutoffFrequency(midHighCutoffFreq);

    m_phaseResponse = static_cast<PhaseResponse>(m_phaseResponseParam->get());
    if (m_phaseResponse == PhaseResponse::linear)
        setLatencySamples(kImpulseSize/2);
    else
        setLatencySamples(0);

    m_LinearPhaseCrossover.setParameters(lowMidCutoffFreq, midHighCutoffFreq);

    m_inputGain = m_inputGainParam->get();
    m_outputGain = m_outputGainParam->get();
    m_postClip = m_masterClipParam->get();


}
void PaxMBClipAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

   

    m_resizedBuffer->setSize(buffer.getNumChannels(), buffer.getNumSamples() * m_oversample, false, true, true);
    m_resizedBuffer->clear();

    updateState();

    //Spectrum Input Meter
    juce::AudioBuffer<float> spectrumInputBuffer;
    spectrumInputBuffer.makeCopyOf(buffer, false);

    juce::dsp::AudioBlock<float> inBlock(spectrumInputBuffer);
    juce::dsp::ProcessContextReplacing<float> inContext(inBlock);
    m_tiltFilter.process(inContext);
    

    spectrumInputBuffer.applyGain(juce::Decibels::decibelsToGain(12.f));
    monoInFifo.update(

        (buffer.getNumChannels() == 1) ? sumBufferToMono(spectrumInputBuffer) : spectrumInputBuffer
    
    );

    if (*m_inputGainParam != m_inputGain)
    {
        auto start = juce::Decibels::decibelsToGain(m_inputGain);
        auto end = juce::Decibels::decibelsToGain(m_inputGainParam->get());
        buffer.applyGainRamp(0, buffer.getNumSamples(), start, end);
        m_inputGain = *m_inputGainParam;
    }
    else
    {
        buffer.applyGain(juce::Decibels::decibelsToGain(m_inputGainParam->get()));
    }

    levelMeterSourceIn.measureBlock(buffer);



    if (m_oversample > 1)
    {
        overSampleZS(&buffer, m_resizedBuffer.get(), buffer.getNumChannels());
        m_resizedBuffer->applyGain(m_oversample);

        m_oversamplingFilter1.process(m_resizedBuffer->getNumSamples(), m_resizedBuffer->getArrayOfWritePointers());
        splitBands(*m_resizedBuffer);

        for (size_t i = 0; i < filterBuffers.size(); ++i)
        {
            clippers[i].process(filterBuffers[i]);
        }

        recombineBands(*m_resizedBuffer);
        m_oversamplingFilter2.process(m_resizedBuffer->getNumSamples(), m_resizedBuffer->getArrayOfWritePointers());

        decimate(m_resizedBuffer.get(), &buffer, buffer.getNumChannels());
    }
    else
    {
        splitBands(buffer);

        for (size_t i = 0; i < filterBuffers.size(); ++i)
        {
            clippers[i].process(filterBuffers[i]);
        }

        recombineBands(buffer);
    }

    if (m_globalBandFocus != BandFocus::unfocused)
    {
        auto waveformDisplayBuffer = sumBufferToMono(filterBuffers[(size_t)m_globalBandFocus - 1]);
        m_waveformDisplay.pushBuffer(waveformDisplayBuffer);
    }
    if (m_postClip)
    {
        m_masterClip.masterClip(&buffer);
    }

    if (*m_outputGainParam != m_outputGain)
    {
        auto start = juce::Decibels::decibelsToGain(m_outputGain);
        auto end = juce::Decibels::decibelsToGain(m_outputGainParam->get());
        buffer.applyGainRamp(0, buffer.getNumSamples(), start, end);
        m_outputGain = *m_outputGainParam;
    }
    else
    {
        buffer.applyGain(juce::Decibels::decibelsToGain(m_outputGainParam->get()));
    }

    // Spectrum Output Meter
    juce::AudioBuffer<float> spectrumOutputBuffer;
    spectrumOutputBuffer.makeCopyOf(buffer, false);

    juce::dsp::AudioBlock<float> outBlock(spectrumOutputBuffer);
    juce::dsp::ProcessContextReplacing<float> outContext(outBlock);
    m_tiltFilter.process(outContext);

    spectrumOutputBuffer.applyGain(juce::Decibels::decibelsToGain(12.f));
    monoOutFifo.update(

        (buffer.getNumChannels() == 1) ? sumBufferToMono(spectrumOutputBuffer) : spectrumOutputBuffer

    );

    levelMeterSourceOut.measureBlock(buffer);

}

void PaxMBClipAudioProcessor::overSampleZS(juce::AudioBuffer<float>* oldBuffer, juce::AudioBuffer<float>* newBuffer, int numchans)
{
    for (int i = 0; i < oldBuffer->getNumSamples(); i++)
    {
        for (int j = 0; j < numchans; j++)
        {
            newBuffer->setSample(j, i * m_oversample, oldBuffer->getSample(j, i));
        }
    }

}

const void PaxMBClipAudioProcessor::decimate(juce::AudioBuffer<float>* upBuffer, juce::AudioBuffer<float>* downBuffer, int numchans)
{
    for (int i = 0; i < downBuffer->getNumSamples(); i++)
    {
        for (int j = 0; j < numchans; j++)
        {
            downBuffer->setSample(j, i, upBuffer->getSample(j, i * m_oversample + (int)(m_sampleShift * (m_oversample - 1))));
        }
    }
}

void PaxMBClipAudioProcessor::splitBands(const juce::AudioBuffer<float>& inputBuffer)
{
    for (auto& fb : filterBuffers)
    {
        fb.clear();
        fb = inputBuffer;
    }

    if (m_phaseResponse == PhaseResponse::minimum)
    {
        auto lowBlock = juce::dsp::AudioBlock<float>(filterBuffers[0]);
        auto midBlock = juce::dsp::AudioBlock<float>(filterBuffers[1]);
        auto highBlock = juce::dsp::AudioBlock<float>(filterBuffers[2]);

        auto lowContext = juce::dsp::ProcessContextReplacing<float>(lowBlock);
        auto midContext = juce::dsp::ProcessContextReplacing<float>(midBlock);
        auto highContext = juce::dsp::ProcessContextReplacing<float>(highBlock);

        LP1.process(lowContext);
        AP2.process(lowContext);
        
        HP1.process(midContext);
        filterBuffers[2] = filterBuffers[1];
        LP2.process(midContext);
        
        HP2.process(highContext);
    }
    else if (m_phaseResponse == PhaseResponse::linear)
    {
        m_LinearPhaseCrossover.processBlock(inputBuffer, filterBuffers[0], filterBuffers[1], filterBuffers[2]);
    }
}

void PaxMBClipAudioProcessor::recombineBands(juce::AudioBuffer<float>& buffer)
{
    auto numSamples = buffer.getNumSamples();
    auto numChannels = buffer.getNumChannels();

    buffer.clear();


    auto bandsAreSoloed = false;
    for (auto& clipper : clippers)
    {
        if (clipper.solo->get())
        {
            bandsAreSoloed = true;
            break;
        }
    }

    if (bandsAreSoloed)
    {
        for (size_t i = 0; i < clippers.size(); ++i)
        {
            auto& clipper = clippers[i];
            if (clipper.solo->get())
            {
                for (auto j = 0; j < filterBuffers[i].getNumChannels(); ++j)
                {
                    buffer.addFrom(j, 0, filterBuffers[i], j, 0, numSamples);
                }
            }
        }
    }
    else
    {
        for (size_t i = 0; i < clippers.size(); ++i)
        {
            auto& clipper = clippers[i];
            if (!clipper.mute->get())
            {
                for (auto j = 0; j < filterBuffers[i].getNumChannels(); ++j)
                {
                    buffer.addFrom(j, 0, filterBuffers[i], j, 0, numSamples);
                }
            }
        }
    }
}

void PaxMBClipAudioProcessor::toggleAnalyzer(bool isOn)
{
    isAnalyzerOn = isOn;
    sendChangeMessage();
}

void PaxMBClipAudioProcessor::setBandFocus(BandFocus inFocus)
{
    m_globalBandFocus = inFocus;
    sendChangeMessage();
}

BandFocus PaxMBClipAudioProcessor::getBandFocus()
{
    return m_globalBandFocus;
}
//==============================================================================
bool PaxMBClipAudioProcessor::hasEditor() const
{
    return true;
}

juce::AudioProcessorEditor* PaxMBClipAudioProcessor::createEditor()
{
    return new PaxMBClipAudioProcessorEditor(*this);
}

//==============================================================================
void PaxMBClipAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    juce::MemoryOutputStream mos(destData, true);
    apvts.state.writeToStream(mos);
}

void PaxMBClipAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    auto tree = juce::ValueTree::readFromData(data, sizeInBytes);
    if (tree.isValid())
    {
        apvts.replaceState(tree);
    }
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new PaxMBClipAudioProcessor();
}

