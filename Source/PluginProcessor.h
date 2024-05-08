
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

    juce::dsp::ProcessorDuplicator<juce::dsp::FIR::Filter<float>, juce::dsp::FIR::Coefficients<float>> FIR1, FIR2, FIR3, FIR4, FIR5;

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

    void applyWindowFunction(std::vector<float>& coefficients)
    {
        // Hamming window
        size_t size = coefficients.size();
        for (size_t i = 0; i < size; ++i)
        {
            float windowValue = 0.54 - 0.46 * cos(2 * M_PI * i / (size - 1));
            coefficients[i] *= windowValue;
        }
    }

    void delayBuffer(juce::AudioBuffer<float>& buffer, int delaySamples)
    {
        const int numChannels = buffer.getNumChannels();
        const int numSamples = buffer.getNumSamples();

        juce::AudioBuffer<float> delayedBuffer(numChannels, numSamples + delaySamples);

        delayedBuffer.clear();
        for (int ch = 0; ch < numChannels; ++ch)
        {
            delayedBuffer.copyFrom(ch, delaySamples, buffer, ch, 0, numSamples);
        }
        buffer = delayedBuffer;
    }


    void createFIRFilters()
    {

        // Vectors to hold the captured impulse responses
        std::vector<float> impulseResponse1, impulseResponse2, impulseResponse3, impulseResponse4, impulseResponse5;

        // Capture the impulse responses
        captureImpulseResponse(LP1, impulseResponse1);
        captureImpulseResponse(AP2, impulseResponse2);
        captureImpulseResponse(HP1, impulseResponse3);
        captureImpulseResponse(LP2, impulseResponse4);
        captureImpulseResponse(HP2, impulseResponse5);

        // Initialize FIR filters using the coefficients
        *FIR1.state = juce::dsp::FIR::Coefficients<float>(impulseResponse1.data(), impulseResponse1.size());
        *FIR2.state = juce::dsp::FIR::Coefficients<float>(impulseResponse2.data(), impulseResponse2.size());
        *FIR3.state = juce::dsp::FIR::Coefficients<float>(impulseResponse3.data(), impulseResponse3.size());
        *FIR4.state = juce::dsp::FIR::Coefficients<float>(impulseResponse4.data(), impulseResponse4.size());
        *FIR5.state = juce::dsp::FIR::Coefficients<float>(impulseResponse5.data(), impulseResponse5.size());
    }

    void captureImpulseResponse(juce::dsp::LinkwitzRileyFilter<float>& filter, std::vector<float>& response)
    {
        response.resize(kImpulseSize, 0.0f);

        // Create an impulse signal
        juce::AudioBuffer<float> impulseBuffer(1, kImpulseSize);
        impulseBuffer.clear();
        impulseBuffer.setSample(0, 0, 1.0f); // Set the first sample to 1 (impulse)

        juce::dsp::AudioBlock<float> block(impulseBuffer);
        juce::dsp::ProcessContextReplacing<float> context(block);
        filter.process(context);

        // Copy the response
        for (int channel = 0; channel < impulseBuffer.getNumChannels(); ++channel)
        {
            for (int i = 0; i < kImpulseSize; ++i)
                response[i] = impulseBuffer.getSample(channel, i);
        }
    }

    void forwardBackwardProcess(juce::AudioBuffer<float>& buffer,
        juce::dsp::ProcessorDuplicator<juce::dsp::FIR::Filter<float>, juce::dsp::FIR::Coefficients<float>>& filter)
    {
        juce::ScopedNoDenormals noDenormals;

        //const int filterLength = filter.state->getFilterOrder();
        const int groupDelay = kImpulseSize - 1;

        const int numChannels = buffer.getNumChannels();
        const int numSamples = buffer.getNumSamples();

        juce::AudioBuffer<float> processingBuffer(numChannels, numSamples + groupDelay*2);
        processingBuffer.clear();

        // initial group delay offset
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
