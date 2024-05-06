
#pragma once

#include <JuceHeader.h>
#include "Params.h"
#include "Clip.h"

#include "Fifo.h"
#include "SingleChannelSampleFifo.h"
#include <array>
#include "DspFilters/Dsp.h"

constexpr int impulseSize = 128;

template<class IIR>
class FIR
{
public:
    FIR(IIR i, size_t firOrder)
        : iir(i)
    {
        firCoeff.resize(firOrder, 0.0f) :
    }
    
    void prepare(const juce::dsp::ProcessSpec& spec)
    {
        firFilter.prepare(spec); 
    }

    void process(const juce::dsp::AudioBlock<float>& block)
    {
        juce::dsp::ProcessContextReplacing<float> context(block);
        firFilter.process(context);
    }

private:

    void recordImpulseResponse() {
        for (int i = 0; i < targetCoeff.size(); i++)
        {
            auto input = i == 0 ? 1.0f : 0.0f;
            firCoeff[i] = iir(input);
        }
    }
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

    void createSymmetricFIR()
    {
        std::vector<float> coefficients;
        coefficients.insert(coefficients.end(), firCoeff.begin(), firCoeff.end());
        coefficients.insert(coefficients.end(), firCoeff.rbegin(), firCoeff.rend());

        applyWindowFunction(coefficients);

        firFilter.setCoefficients(coefficients);
    }

    IIR iir;
    std::vector<float> firCoeff;
    juce::dsp::FIR::Filter<float> firFilter;
};

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
    static const int m_forder = 12;
    const int m_maxOversample = 32;
    const float m_sampleShift = 0.0;


    std::array<juce::AudioBuffer<float>, 3> filterBuffers;
    std::unique_ptr<juce::AudioBuffer<float>> m_resizedBuffer;


    // Minimum Phase Crossover Filters
    using minimumPhaseFilter = juce::dsp::LinkwitzRileyFilter<float>;
    //                  fc0  fc1
    minimumPhaseFilter  LP1, AP2,
                        HP1, LP2,
                        HP2;

    juce::dsp::FIR::Filter<float> FIR1, FIR2, FIR3, FIR4, FIR5;

    // Linear Phase Crossover Filters
    //using linearPhaseFilter = Falco::Dsp::SimpleFilter <Falco::Dsp::Bessel::LowPass <m_forder>, 2>;
    //linearPhaseFilter iLP3, iAP4,
    //                  iHP3, iLP4,
    //                  iHP4;

   // FIR<IIRFilter> fLP3, fAP4,
   //                fHP3, fLP4,
   //                fHP4;

    // OversamplingiHP4; filters (butterworth)
    using OversamplingFilter = Dsp::SimpleFilter <Dsp::Butterworth::LowPass <m_forder>, 2>;
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

        // Vectors to hold the captured impulse responses
        std::vector<float> impulseResponse1, impulseResponse2, impulseResponse3, impulseResponse4, impulseResponse5;

        // Capture the impulse responses
        captureImpulseResponse(LP1, impulseResponse1);
        captureImpulseResponse(AP2, impulseResponse2);
        captureImpulseResponse(HP1, impulseResponse3);
        captureImpulseResponse(LP2, impulseResponse4);
        captureImpulseResponse(HP2, impulseResponse5);

        // Create FIR coefficients from the impulse responses
        auto coefficients1 = std::make_shared<juce::dsp::FIR::Coefficients<float>>(impulseResponse1.data(), impulseResponse1.size());
        auto coefficients2 = std::make_shared<juce::dsp::FIR::Coefficients<float>>(impulseResponse2.data(), impulseResponse2.size());
        auto coefficients3 = std::make_shared<juce::dsp::FIR::Coefficients<float>>(impulseResponse3.data(), impulseResponse3.size());
        auto coefficients4 = std::make_shared<juce::dsp::FIR::Coefficients<float>>(impulseResponse4.data(), impulseResponse4.size());
        auto coefficients5 = std::make_shared<juce::dsp::FIR::Coefficients<float>>(impulseResponse5.data(), impulseResponse5.size());

        // Initialize FIR filters using the coefficients
        FIR1.coefficients = juce::ReferenceCountedObjectPtr(coefficients1.get());
        FIR2.coefficients = juce::ReferenceCountedObjectPtr(coefficients2.get());
        FIR3.coefficients = juce::ReferenceCountedObjectPtr(coefficients3.get());
        FIR4.coefficients = juce::ReferenceCountedObjectPtr(coefficients4.get());
        FIR5.coefficients = juce::ReferenceCountedObjectPtr(coefficients5.get());
    }

    void captureImpulseResponse(juce::dsp::LinkwitzRileyFilter<float>& filter, std::vector<float>& response)
    {
        response.resize(impulseSize, 0.0f);

        // Create an impulse signal
        juce::AudioBuffer<float> impulseBuffer(1, impulseSize);
        impulseBuffer.clear();
        impulseBuffer.setSample(0, 0, 1.0f); // Set the first sample to 1 (impulse)

        juce::dsp::AudioBlock<float> block(impulseBuffer);
        juce::dsp::ProcessContextReplacing<float> context(block);

        filter.prepare({ 44100, (juce::uint32)impulseSize, 1 });
        filter.process(context);

        // Copy the response
        for (int i = 0; i < impulseSize; ++i)
            response[i] = impulseBuffer.getSample(0, i);
    }





    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PaxMBClipAudioProcessor)
};
