/*
  =============================================================================
     

  Chomp - Multiband Clipper VST

  TO DO:
  
  - add oversampling and post clip artefact logic
  - add level meters 
  ? scroll wheel for clip controls
  ? add Overhead font and gater aesthetic

  - add support for waveform analyzer
    - switch analysis with current analyzer button in cb
    ? possible different display without bands

  - design release GUI
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
    )
#endif
{

    initializeParameters();


    //addParameter(waveType = new juce::AudioParameterInt("wavetype", "Wavetype", 0, 5, 0));
}

PaxMBClipAudioProcessor::~PaxMBClipAudioProcessor()
{
}

void PaxMBClipAudioProcessor::initializeParameters()
{
    using namespace Params;
    const auto& params = GetParams();

    // Float and int parameter initialization
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

    inputGainParam = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(Names::Gain_In)));
    outputGainParam = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(Names::Gain_Out)));

    // Boolean parameter initialization
    lowBandClip.bypassed = dynamic_cast<juce::AudioParameterBool*>(apvts.getParameter(params.at(Names::Bypassed_Low)));
    midBandClip.bypassed = dynamic_cast<juce::AudioParameterBool*>(apvts.getParameter(params.at(Names::Bypassed_Mid)));
    highBandClip.bypassed = dynamic_cast<juce::AudioParameterBool*>(apvts.getParameter(params.at(Names::Bypassed_High)));

    lowBandClip.mute = dynamic_cast<juce::AudioParameterBool*>(apvts.getParameter(params.at(Names::Mute_Low)));
    midBandClip.mute = dynamic_cast<juce::AudioParameterBool*>(apvts.getParameter(params.at(Names::Mute_Mid)));
    highBandClip.mute = dynamic_cast<juce::AudioParameterBool*>(apvts.getParameter(params.at(Names::Mute_High)));

    lowBandClip.solo = dynamic_cast<juce::AudioParameterBool*>(apvts.getParameter(params.at(Names::Solo_Low)));
    midBandClip.solo = dynamic_cast<juce::AudioParameterBool*>(apvts.getParameter(params.at(Names::Solo_Mid)));
    highBandClip.solo = dynamic_cast<juce::AudioParameterBool*>(apvts.getParameter(params.at(Names::Solo_High)));
}

juce::AudioProcessorValueTreeState::ParameterLayout PaxMBClipAudioProcessor::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    using namespace Params;
    const auto& params = GetParams();

    layout.add(std::make_unique<juce::AudioParameterFloat>(params.at(Names::Low_Mid_Crossover_Freq), params.at(Names::Low_Mid_Crossover_Freq), juce::NormalisableRange<float> (PaxMBClip::MIN_FREQUENCY, PaxMBClip::MAX_FREQUENCY, 1.f, 0.2f), 200.f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(params.at(Names::Mid_High_Crossover_Freq), params.at(Names::Mid_High_Crossover_Freq), juce::NormalisableRange<float>(PaxMBClip::MIN_FREQUENCY, PaxMBClip::MAX_FREQUENCY, 1.f, 0.2f), 2000.f));

    auto clipLow = -48.0f;
    auto clipHigh = 0.0f;
    
    auto gainLow = -24.0f;
    auto gainHigh = 24.0f;

    layout.add(std::make_unique<juce::AudioParameterFloat>(params.at(Names::Low_Clip), params.at(Names::Low_Clip), clipLow, clipHigh, 0.f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(params.at(Names::Mid_Clip), params.at(Names::Mid_Clip), clipLow, clipHigh, 0.f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(params.at(Names::High_Clip), params.at(Names::High_Clip), clipLow, clipHigh, 0.f));

    layout.add(std::make_unique<juce::AudioParameterFloat>(params.at(Names::Gain_In), params.at(Names::Gain_In), gainLow, gainHigh, 0.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(params.at(Names::Gain_Out), params.at(Names::Gain_Out), gainLow, gainHigh, 0.0f));

    layout.add(std::make_unique<juce::AudioParameterFloat>(params.at(Names::Low_Gain), params.at(Names::Low_Gain), gainLow, gainHigh, 0.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(params.at(Names::Mid_Gain), params.at(Names::Mid_Gain), gainLow, gainHigh, 0.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(params.at(Names::High_Gain), params.at(Names::High_Gain), gainLow, gainHigh, 0.0f));

    layout.add(std::make_unique<juce::AudioParameterInt>(params.at(Names::Low_Wave), params.at(Names::Low_Wave), 0, 5, 0));
    layout.add(std::make_unique<juce::AudioParameterInt>(params.at(Names::Mid_Wave), params.at(Names::Mid_Wave), 0, 5, 0));
    layout.add(std::make_unique<juce::AudioParameterInt>(params.at(Names::High_Wave), params.at(Names::High_Wave), 0, 5, 0));

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

    for (auto& clipper : clippers)
    {
        clipper.prepare(spec);
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

    leftChannelFifo.prepare(samplesPerBlock);
    rightChannelFifo.prepare(samplesPerBlock);
}

void PaxMBClipAudioProcessor::releaseResources()
{
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool PaxMBClipAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
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

    updateState();
    leftChannelFifo.update(buffer);
    rightChannelFifo.update(buffer);

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
                addFilterBand(buffer, filterBuffers[i]);
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
                addFilterBand(buffer, filterBuffers[i]);
            }
        }
    }

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

void PaxMBClipAudioProcessor::setBandFocus(BandFocus inFocus)
{
    globalBandFocus = inFocus;
    sendChangeMessage();
}

BandFocus PaxMBClipAudioProcessor::getBandFocus()
{
    return globalBandFocus;
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

