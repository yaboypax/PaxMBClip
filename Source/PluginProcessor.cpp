/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
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
    )
#endif
{
    using namespace Params;
    const auto& params = GetParams();

    auto floatHelper = [&apvts = this->apvts, &params](auto& param, const auto& paramName)
    {
        param = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(paramName)));
        jassert(param != nullptr);
    };

    floatHelper(lowBandClip.gain, Names::Low_Gain);
    floatHelper(lowBandClip.clip, Names::Low_Clip);

    floatHelper(midBandClip.gain, Names::Mid_Gain);
    floatHelper(midBandClip.clip, Names::Mid_Clip);

    floatHelper(highBandClip.gain, Names::High_Gain);
    floatHelper(highBandClip.clip, Names::High_Clip);

    floatHelper(lowMidCrossover, Names::Low_Mid_Crossover_Freq);
    floatHelper(midHighCrossover, Names::Mid_High_Crossover_Freq);

    floatHelper(inputGainParam, Names::Gain_In);
    floatHelper(outputGainParam, Names::Gain_Out);

    auto boolHelper = [&apvts = this->apvts, &params](auto& param, const auto& paramName)
    {
        param = dynamic_cast<juce::AudioParameterBool*>(apvts.getParameter(params.at(paramName)));
        jassert(param != nullptr);
    };

    boolHelper(lowBandClip.bypassed, Names::Bypassed_Low);
    boolHelper(midBandClip.bypassed, Names::Bypassed_Mid);
    boolHelper(highBandClip.bypassed, Names::Bypassed_High);

    boolHelper(lowBandClip.mute, Names::Mute_Low);
    boolHelper(midBandClip.mute, Names::Mute_Mid);
    boolHelper(highBandClip.mute, Names::Mute_High);

    boolHelper(lowBandClip.solo, Names::Solo_Low);
    boolHelper(midBandClip.solo, Names::Solo_Mid);
    boolHelper(highBandClip.solo, Names::Solo_High);

}

PaxMBClipAudioProcessor::~PaxMBClipAudioProcessor()
{
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
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int PaxMBClipAudioProcessor::getCurrentProgram()
{
    return 0;
}

void PaxMBClipAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String PaxMBClipAudioProcessor::getProgramName (int index)
{
    return {};
}

void PaxMBClipAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void PaxMBClipAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    juce::dsp::ProcessSpec spec;
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = getTotalNumOutputChannels();
    spec.sampleRate = sampleRate;

    for (int i = 0; i < 3; ++i)
    {
        clippers[i].prepare(spec);
    }

    LP1.prepare(spec);
    HP1.prepare(spec);
    LP2.prepare(spec);
    HP2.prepare(spec);
    AP2.prepare(spec);

    inputGain.prepare(spec);
    outputGain.prepare(spec);

    inputGain.setRampDurationSeconds(0.05); //50 ms
    outputGain.setRampDurationSeconds(0.05);

    setCrossoverFilters();

    for (auto& buffer : filterBuffers)
    {
        buffer.setSize(spec.numChannels, samplesPerBlock);
    }
}

void PaxMBClipAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool PaxMBClipAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
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

    auto lowMidCutoffFreq = lowMidCrossover->get();
    LP1.setCutoffFrequency(lowMidCutoffFreq);
    HP1.setCutoffFrequency(lowMidCutoffFreq);

    auto midHighCutoffFreq = midHighCrossover->get();
    AP2.setCutoffFrequency(midHighCutoffFreq);
    LP2.setCutoffFrequency(midHighCutoffFreq);
    HP2.setCutoffFrequency(midHighCutoffFreq);

    inputGain.setGainDecibels(inputGainParam->get());
    outputGain.setGainDecibels(outputGainParam->get());

}
void PaxMBClipAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    for (auto& clipper : clippers)
    {
        clipper.updateClipperSettings();
    }

    updateState();

    applyGain(buffer, inputGain);

    splitBands(buffer);

    for (size_t i = 0; i < filterBuffers.size(); ++i) 
    {
        clippers[i].process(filterBuffers[i]);
    }


    //context.isBypassed = bypassedLow->get();

    auto numSamples = buffer.getNumSamples();
    auto numChannels = buffer.getNumChannels();

    buffer.clear();

    auto addFilterBand = [numSamples, numChannels](auto& inputBuffer, const auto& source)
    {
        for (auto i = 0; i < numChannels; ++i)
        {
            inputBuffer.addFrom(i, 0, source, i, 0, numSamples);
        }
    };

    addFilterBand(buffer, filterBuffers[0]);
    addFilterBand(buffer, filterBuffers[1]);
    addFilterBand(buffer, filterBuffers[2]);

    applyGain(buffer, outputGain);
}

void PaxMBClipAudioProcessor::splitBands(const juce::AudioBuffer<float>& inputBuffer)
{
    for (auto& fb : filterBuffers)
    {
        fb = inputBuffer;
    }

    auto fb0Block = juce::dsp::AudioBlock<float>(filterBuffers[0]);
    auto fb1Block = juce::dsp::AudioBlock<float>(filterBuffers[1]);
    auto fb2Block = juce::dsp::AudioBlock<float>(filterBuffers[2]);

    auto fb0Ctx = juce::dsp::ProcessContextReplacing<float>(fb0Block);
    auto fb1Ctx = juce::dsp::ProcessContextReplacing<float>(fb1Block);
    auto fb2Ctx = juce::dsp::ProcessContextReplacing<float>(fb2Block);

    LP1.process(fb0Ctx);
    AP2.process(fb0Ctx);

    HP1.process(fb1Ctx);
    filterBuffers[2] = filterBuffers[1];
    LP2.process(fb1Ctx);

    HP2.process(fb2Ctx);
}
//==============================================================================
bool PaxMBClipAudioProcessor::hasEditor() const
{
    return true;
}

juce::AudioProcessorEditor* PaxMBClipAudioProcessor::createEditor()
{
    return new PaxMBClipAudioProcessorEditor (*this);
}

//==============================================================================
void PaxMBClipAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    juce::MemoryOutputStream mos(destData, true);
    apvts.state.writeToStream(mos);
}

void PaxMBClipAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
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

juce::AudioProcessorValueTreeState::ParameterLayout PaxMBClipAudioProcessor::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    using namespace Params;
    const auto& params = GetParams();

    layout.add(std::make_unique<juce::AudioParameterFloat>(params.at(Names::Low_Mid_Crossover_Freq), params.at(Names::Low_Mid_Crossover_Freq), juce::NormalisableRange<float>(20, 20000, 1, 1), 200));
    layout.add(std::make_unique<juce::AudioParameterFloat>(params.at(Names::Mid_High_Crossover_Freq), params.at(Names::Mid_High_Crossover_Freq), juce::NormalisableRange<float>(20, 20000, 1, 1), 2000));

    layout.add(std::make_unique<juce::AudioParameterFloat>(params.at(Names::Low_Clip), params.at(Names::Low_Clip), juce::NormalisableRange<float>(0, 1, 0.01, 1), 0));
    layout.add(std::make_unique<juce::AudioParameterFloat>(params.at(Names::Mid_Clip), params.at(Names::Mid_Clip), juce::NormalisableRange<float>(0, 1, 0.01, 1), 0));
    layout.add(std::make_unique<juce::AudioParameterFloat>(params.at(Names::High_Clip), params.at(Names::High_Clip), juce::NormalisableRange<float>(0, 1, 0.01, 1), 0));

    auto gainRange = juce::NormalisableRange<float>(-24.f, 24.f, 0.5f, 1.f);
    layout.add(std::make_unique<juce::AudioParameterFloat>(params.at(Names::Gain_In), params.at(Names::Gain_In), gainRange, 0));
    layout.add(std::make_unique<juce::AudioParameterFloat>(params.at(Names::Gain_Out), params.at(Names::Gain_Out), gainRange, 0));

    layout.add(std::make_unique<juce::AudioParameterFloat>(params.at(Names::Low_Gain), params.at(Names::Low_Gain), gainRange, 0));
    layout.add(std::make_unique<juce::AudioParameterFloat>(params.at(Names::Mid_Gain), params.at(Names::Mid_Gain), gainRange, 0));
    layout.add(std::make_unique<juce::AudioParameterFloat>(params.at(Names::High_Gain), params.at(Names::High_Gain), gainRange, 0));

    layout.add(std::make_unique<juce::AudioParameterBool>(params.at(Names::Bypassed_Low), params.at(Names::Bypassed_Low), false));
    layout.add(std::make_unique<juce::AudioParameterBool>(params.at(Names::Bypassed_Mid), params.at(Names::Bypassed_Mid), false));
    layout.add(std::make_unique<juce::AudioParameterBool>(params.at(Names::Bypassed_High), params.at(Names::Bypassed_High), false));

    layout.add(std::make_unique<juce::AudioParameterBool>(params.at(Names::Mute_Low), params.at(Names::Mute_Low), false));
    layout.add(std::make_unique<juce::AudioParameterBool>(params.at(Names::Mute_Mid), params.at(Names::Mute_Mid), false));
    layout.add(std::make_unique<juce::AudioParameterBool>(params.at(Names::Mute_High), params.at(Names::Mute_High), false));

    layout.add(std::make_unique<juce::AudioParameterBool>(params.at(Names::Solo_Low), params.at(Names::Solo_Low), false));
    layout.add(std::make_unique<juce::AudioParameterBool>(params.at(Names::Solo_Mid), params.at(Names::Solo_Mid), false));
    layout.add(std::make_unique<juce::AudioParameterBool>(params.at(Names::Solo_High), params.at(Names::Solo_High), false));

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