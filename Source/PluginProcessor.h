
#pragma once

#include <JuceHeader.h>
#include "Params.h"
#include "Clip.h"

#include "Fifo.h"
#include "SingleChannelSampleFifo.h"
#include <array>
#include "DspFilters/Dsp.h"

namespace
{
    constexpr int kImpulseSize = 128;
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
    //Linear Phase Filters
    using FIRFilter = juce::dsp::ProcessorDuplicator<juce::dsp::FIR::Filter<float>, juce::dsp::FIR::Coefficients<float>>;
    juce::dsp::ProcessorChain<FIRFilter, FIRFilter> FIR1, FIR2, FIR3, FIR4, FIR5;

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

    void createFIRFilters()
    {
        std::vector<float> impulseResponse1L, impulseResponse1R;
        std::vector<float> impulseResponse2L, impulseResponse2R;
        std::vector<float> impulseResponse3L, impulseResponse3R;
        std::vector<float> impulseResponse4L, impulseResponse4R;
        std::vector<float> impulseResponse5L, impulseResponse5R;

        // Capture the impulse responses for each filter and channel
        captureImpulseResponse(LP1, impulseResponse1L, impulseResponse1R);
        captureImpulseResponse(AP2, impulseResponse2L, impulseResponse2R);
        captureImpulseResponse(HP1, impulseResponse3L, impulseResponse3R);
        captureImpulseResponse(LP2, impulseResponse4L, impulseResponse4R);
        captureImpulseResponse(HP2, impulseResponse5L, impulseResponse5R);

        // Initialize FIR filters using the coefficients
        *FIR1.get<0>().state = juce::dsp::FIR::Coefficients<float>(impulseResponse1L.data(), impulseResponse1L.size());
        *FIR1.get<1>().state = juce::dsp::FIR::Coefficients<float>(impulseResponse1R.data(), impulseResponse1R.size());

        *FIR2.get<0>().state = juce::dsp::FIR::Coefficients<float>(impulseResponse2L.data(), impulseResponse2L.size());
        *FIR2.get<1>().state = juce::dsp::FIR::Coefficients<float>(impulseResponse2R.data(), impulseResponse2R.size());

        *FIR3.get<0>().state = juce::dsp::FIR::Coefficients<float>(impulseResponse3L.data(), impulseResponse3L.size());
        *FIR3.get<1>().state = juce::dsp::FIR::Coefficients<float>(impulseResponse3R.data(), impulseResponse3R.size());

        *FIR4.get<0>().state = juce::dsp::FIR::Coefficients<float>(impulseResponse4L.data(), impulseResponse4L.size());
        *FIR4.get<1>().state = juce::dsp::FIR::Coefficients<float>(impulseResponse4R.data(), impulseResponse4R.size());

        *FIR5.get<0>().state = juce::dsp::FIR::Coefficients<float>(impulseResponse5L.data(), impulseResponse5L.size());
        *FIR5.get<1>().state = juce::dsp::FIR::Coefficients<float>(impulseResponse5R.data(), impulseResponse5R.size());
    }

    void captureImpulseResponse(juce::dsp::LinkwitzRileyFilter<float>& filter, std::vector<float>& responseLeft, std::vector<float>& responseRight)
    {
        responseLeft.resize(kImpulseSize, 0.0f);
        responseRight.resize(kImpulseSize, 0.0f);

        // Create an impulse signal for both channels
        juce::AudioBuffer<float> impulseBuffer(2, kImpulseSize);
        impulseBuffer.clear();
        impulseBuffer.setSample(0, 0, 1.0f); // Set the first sample to 1 for the left channel
        impulseBuffer.setSample(1, 0, 1.0f); // Set the first sample to 1 for the right channel

        juce::dsp::AudioBlock<float> block(impulseBuffer);
        juce::dsp::ProcessContextReplacing<float> context(block);
        filter.process(context);

        // Copy the response for left and right channels separately
        for (int i = 0; i < kImpulseSize; ++i)
        {
            responseLeft[i] = impulseBuffer.getSample(0, i);
            responseRight[i] = impulseBuffer.getSample(1, i);
        }
    }

    void forwardBackwardProcess(juce::AudioBuffer<float>& buffer,
        juce::dsp::ProcessorChain<FIRFilter, FIRFilter>& filter)
    {
        juce::ScopedNoDenormals noDenormals;
        const int groupDelay = kImpulseSize / 2;

        const int numChannels = buffer.getNumChannels();
        const int numSamples = buffer.getNumSamples();

        juce::AudioBuffer<float> processingBuffer(numChannels, numSamples + 2 * groupDelay);
        processingBuffer.clear();

        // Initial group delay offset
        for (int channel = 0; channel < numChannels; ++channel)
        {
            processingBuffer.copyFrom(channel, groupDelay, buffer, channel, 0, numSamples);
        }

        // Forward
        auto block = juce::dsp::AudioBlock<float>(processingBuffer);
        auto context = juce::dsp::ProcessContextReplacing<float>(block);

        filter.process(context);
        processingBuffer.reverse(0, processingBuffer.getNumSamples());

        // Reverse
        auto reverseBlock = juce::dsp::AudioBlock<float>(processingBuffer);
        auto reverseContext = juce::dsp::ProcessContextReplacing<float>(reverseBlock);

        filter.process(reverseContext);
        processingBuffer.reverse(0, processingBuffer.getNumSamples());

        // Remove group delay
        for (int channel = 0; channel < numChannels; ++channel)
        {
            buffer.copyFrom(channel, 0, processingBuffer, channel, groupDelay, numSamples);
        }
    }





    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PaxMBClipAudioProcessor)
};
