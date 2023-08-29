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
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
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

void PaxMBClipAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    // Make sure to reset the state if your inner loop is processing
    // the samples and the outer loop is handling the channels.
    // Alternatively, you can process the samples with the channels
    // interleaved by keeping the same state.
    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer (channel);

        // ..do something to the data...
    }
}

//==============================================================================
bool PaxMBClipAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* PaxMBClipAudioProcessor::createEditor()
{
    return new PaxMBClipAudioProcessorEditor (*this);
}

//==============================================================================
void PaxMBClipAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void PaxMBClipAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
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

    layout.add(std::make_unique<juce::AudioParameterFloat>(params.at(Names::Low_Gain), params.at(Names::Low_Gain), juce::NormalisableRange<float>(0, 1, 0.01, 1), 0));
    layout.add(std::make_unique<juce::AudioParameterFloat>(params.at(Names::Mid_Gain), params.at(Names::Mid_Gain), juce::NormalisableRange<float>(0, 1, 0.01, 1), 0));
    layout.add(std::make_unique<juce::AudioParameterFloat>(params.at(Names::High_Gain), params.at(Names::High_Gain), juce::NormalisableRange<float>(0, 1, 0.01, 1), 0));

    layout.add(std::make_unique<juce::AudioParameterBool>(params.at(Names::Bypassed_Low), params.at(Names::Bypassed_Low), false));
    layout.add(std::make_unique<juce::AudioParameterBool>(params.at(Names::Bypassed_Mid), params.at(Names::Bypassed_Mid), false));
    layout.add(std::make_unique<juce::AudioParameterBool>(params.at(Names::Bypassed_High), params.at(Names::Bypassed_High), false));

    layout.add(std::make_unique<juce::AudioParameterBool>(params.at(Names::Mute_Low), params.at(Names::Mute_Low), false));
    layout.add(std::make_unique<juce::AudioParameterBool>(params.at(Names::Mute_Mid), params.at(Names::Mute_Mid), false));
    layout.add(std::make_unique<juce::AudioParameterBool>(params.at(Names::Mute_High), params.at(Names::Mute_High), false));

    layout.add(std::make_unique<juce::AudioParameterBool>(params.at(Names::Solo_Low), params.at(Names::Solo_Low), false));
    layout.add(std::make_unique<juce::AudioParameterBool>(params.at(Names::Solo_Mid), params.at(Names::Solo_Mid), false));
    layout.add(std::make_unique<juce::AudioParameterBool>(params.at(Names::Solo_High), params.at(Names::Solo_High), false));

    layout.add(std::make_unique<juce::AudioParameterFloat>(params.at(Names::Gain_In), params.at(Names::Gain_In), juce::NormalisableRange<float>(0, 1, 0.01, 1), 0));
    layout.add(std::make_unique<juce::AudioParameterFloat>(params.at(Names::Gain_Out), params.at(Names::Gain_Out), juce::NormalisableRange<float>(0, 1, 0.01, 1), 0));

    return layout;
}
