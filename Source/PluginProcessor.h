
#pragma once

#include <JuceHeader.h>
#include "Params.h"
#include "Clip.h"
#include "utilities.h"

#include "Fifo.h"
#include "SingleChannelSampleFifo.h"
#include <array>

#include "DspFilters/Dsp.h"
#define F_ORDER 12 


//====================================================================
enum class BandFocus
{
    unfocused=0,
    Low,
    Mid,
    High
};

class PaxMBClipAudioProcessor  : public juce::AudioProcessor, public juce::ChangeBroadcaster
                            #if JucePlugin_Enable_ARA
                             , public juce::AudioProcessorARAExtension
                            #endif 
{
public:
    //==============================================================================
    PaxMBClipAudioProcessor();
    ~PaxMBClipAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
    void splitBands(const juce::AudioBuffer<float>& inputBuffer);
    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;
    void updateState();
    
    void initializeParameters();
    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    juce::AudioProcessorValueTreeState apvts{ *this, nullptr, "Parameters", createParameterLayout() };

    juce::AudioParameterFloat* lowMidCrossover{ nullptr };
    juce::AudioParameterFloat* midHighCrossover{ nullptr };
    void setCrossoverFilters();
    void setBandFocus(BandFocus inFocus);
    BandFocus getBandFocus();

    float calcCutoff();
    double m_fsamplerate;


    template<typename T, typename U>
    void applyGain(T& buffer, U& dsp)
    {
        auto block = juce::dsp::AudioBlock<float>(buffer);
        auto ctx = juce::dsp::ProcessContextReplacing<float>(block);
        dsp.process(ctx);
    }

    using BlockType = juce::AudioBuffer<float>;
    SingleChannelSampleFifo<BlockType> leftChannelFifo{ Channel::Left };
    SingleChannelSampleFifo<BlockType> rightChannelFifo{ Channel::Right };

private:
    //==============================================================================
    // Crossover Filters
    using xOverFilter = juce::dsp::LinkwitzRileyFilter<float>;
    //           fc0  fc1
    xOverFilter  LP1, AP2,
                 HP1, LP2,
                 HP2;

    std::array<juce::AudioBuffer<float>, 3> filterBuffers;

    // Oversampling filters (butterworth)
    using oversamplingFilter = juce::dsp::IIR::Filter<float>;
    oversamplingFilter oFilter1, oFilter2;
    int m_oversample = 1;
    juce::AudioBuffer<float>* m_resizedBuffer;


    float m_inputGain, m_outputGain;
    juce::AudioParameterFloat* inputGainParam{ nullptr };
    juce::AudioParameterFloat* outputGainParam{ nullptr };

    BandFocus m_globalBandFocus = BandFocus::unfocused;

    std::array<Clipper, 3> clippers;
    Clipper& lowBandClip = clippers[0];
    Clipper& midBandClip = clippers[1];
    Clipper& highBandClip = clippers[2];

    int m_forder = F_ORDER;
    void overSampleZS(juce::AudioBuffer<float>* oldBuffer, juce::AudioSampleBuffer* newBuffer, int numchans);
    void decimate(juce::AudioBuffer<float>* upBuffer, juce::AudioSampleBuffer* downBuffer, int numchans);
    const int m_maxOversample = 32;
    const float m_sampleShift = 0.0;


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PaxMBClipAudioProcessor)
};
