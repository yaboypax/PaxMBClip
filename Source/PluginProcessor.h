
#pragma once

#include <JuceHeader.h>
#include "Params.h"
#include "Clip.h"

#include "Fifo.h"
#include "SingleChannelSampleFifo.h"
#include <array>
#include "DspFilters/Dsp.h"
#include <chowdsp_eq/EQ/chowdsp_LinearPhaseEQ.h>
#include "LinearPhaseEQ.h"

namespace
{
    constexpr int kFOrder = 12;
    constexpr int kMaxOversample = 32;
}

//====================================================================
enum class BandFocus
{
    unfocused = 0,
    Low,
    Mid,
    High
};

enum class PhaseResponse
{
    linear = 0,
    minimum
};


class PaxMBClipAudioProcessor : public juce::AudioProcessor, public juce::ChangeBroadcaster
#if JucePlugin_Enable_ARA
    , public juce::AudioProcessorARAExtension
#endif 
{
public:
    //==============================================================================
    PaxMBClipAudioProcessor();
    ~PaxMBClipAudioProcessor() override;

    juce::Point<int> m_savedSize;
    juce::Point<int> getSavedSize() { return m_savedSize; }
    void setSavedSize(int x, int y) { m_savedSize = { x, y }; }


    //==============================================================================
    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

#ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;
#endif

    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
    void splitBands(const juce::AudioBuffer<float>& inputBuffer);
    void recombineBands(juce::AudioBuffer<float>& buffer);

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
    void setCurrentProgram(int index) override;
    const juce::String getProgramName(int index) override;
    void changeProgramName(int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;
    void updateState();

    void initializeParameters();
    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    juce::AudioProcessorValueTreeState apvts{ *this, nullptr, "Parameters", createParameterLayout() };

    juce::AudioParameterFloat* lowMidCrossover{ nullptr };
    juce::AudioParameterFloat* midHighCrossover{ nullptr };
    void setCrossoverFilters();

    void setBandFocus(BandFocus inFocus);
    BandFocus getBandFocus();


    template<typename T, typename U>
    void applyGain(T& buffer, U& dsp)
    {
        auto block = juce::dsp::AudioBlock<float>(buffer);
        auto ctx = juce::dsp::ProcessContextReplacing<float>(block);
        dsp.process(ctx);
    }

    using BlockType = juce::AudioBuffer<float>;
    SingleChannelSampleFifo<BlockType> monoInFifo{ Channel::Left };
    SingleChannelSampleFifo<BlockType> rightInFifo{ Channel::Right };

    SingleChannelSampleFifo<BlockType> monoOutFifo{ Channel::Left };
    SingleChannelSampleFifo<BlockType> rightOutFifo{ Channel::Right };

    LevelMeterSource levelMeterSourceIn;
    LevelMeterSource levelMeterSourceOut;
    LevelMeterSource& getMeterSourceIn() { return levelMeterSourceIn; }
    LevelMeterSource& getMeterSourceOut() { return levelMeterSourceOut; }

    void toggleAnalyzer(bool isOn);
    bool getAnalyzerOn() { return isAnalyzerOn; }
    juce::AudioVisualiserComponent m_waveformDisplay;

private:
    //==============================================================================
    const float m_sampleShift = 0.0;


    std::array<juce::AudioBuffer<float>, 3> filterBuffers;
    std::unique_ptr<juce::AudioBuffer<float>> m_resizedBuffer;

    

    // Minimum Phase Crossover Filters
    using minimumPhaseFilter = juce::dsp::LinkwitzRileyFilter<float>;
    //                  fc0  fc1
    minimumPhaseFilter  LP1, AP2,
                        HP1, LP2,
                        HP2;
    
    //Linear Phase Filters (chowDSP)
    chowdsp::LinearPhase3WayCrossover<20> m_LinearPhaseCrossover;

    // OversamplingiHP4; filters (butterworth)
    using OversamplingFilter = Dsp::SimpleFilter <Dsp::Butterworth::LowPass <kFOrder>, 2>;
    OversamplingFilter m_oversamplingFilter1, m_oversamplingFilter2;

    // tilt filters for spectrum display
    using TiltFilter = juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>, juce::dsp::IIR::Coefficients<float>>;
    juce::dsp::ProcessorChain<TiltFilter, TiltFilter> m_tiltFilter;


    void overSampleZS(juce::AudioBuffer<float>* oldBuffer, juce::AudioBuffer<float>* newBuffer, int numchans);
    const void decimate(juce::AudioBuffer<float>* upBuffer, juce::AudioBuffer<float>* downBuffer, int numchans);

    int m_oversample = 1;
    juce::AudioParameterInt* m_oversampleParam{ nullptr };

    float m_inputGain, m_outputGain;
    juce::AudioParameterFloat* m_inputGainParam{ nullptr };
    juce::AudioParameterFloat* m_outputGainParam{ nullptr };

    BandFocus m_globalBandFocus = BandFocus::unfocused;
    PhaseResponse m_phaseResponse = PhaseResponse::minimum;
    juce::AudioParameterInt* m_phaseResponseParam{ nullptr };

    std::array<Clipper, 3> clippers;
    Clipper& lowBandClip = clippers[0];
    Clipper& midBandClip = clippers[1];
    Clipper& highBandClip = clippers[2];


    Clipper m_masterClip;
    bool m_postClip = false;
    juce::AudioParameterBool* m_masterClipParam{ nullptr };
    bool isAnalyzerOn = true;  



    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PaxMBClipAudioProcessor)
};
