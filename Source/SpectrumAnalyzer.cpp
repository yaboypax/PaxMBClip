/*
  ==============================================================================

    SpectrumAnalyzer.cpp
    Created: 29 Aug 2023 10:14:08pm
    Author:  Paxton Fleming

  ==============================================================================
*/

#include "SpectrumAnalyzer.h"
#include "Params.h"
#include "Utilities.h"

namespace
{
    constexpr int marginY = 32;
    constexpr int size = 25;
    constexpr int muteOffset = 51;
    constexpr int soloOffset = 27;

    constexpr int bandMargin = 10;
    constexpr int bandWidth = 150;
    constexpr int bandHeight = MIN_HEIGHT - 190;
}

SpectrumAnalyzer::SpectrumAnalyzer(PaxMBClipAudioProcessor& p) :
    m_processor(p),
    inPathProducer(m_processor.monoInFifo),
    outPathProducer(m_processor.monoOutFifo)
{
    const auto& params = m_processor.getParameters();
    for (auto param : params)
    {
        param->addListener(this);
    }

    layoutBandButtons();
    setAttachments();
    addChildComponent(m_frequencyDisplay);

    addChildComponent(bandControls);
    bandControls.setLookAndFeel(chompLAF);

    addChildComponent(m_bandLabel);
    m_bandLabel.setLookAndFeel(chompLAF);
    
    m_processor.addChangeListener(this);
    addChildComponent(m_processor.m_waveformDisplay);
    startTimerHz(40);
}

SpectrumAnalyzer::~SpectrumAnalyzer()
{
    const auto& params = m_processor.getParameters();
    for (auto param : params)
    {
        param->removeListener(this);
    }
    m_processor.removeChangeListener(this);

    m_soloLowButton.removeListener(this);
    m_soloMidButton.removeListener(this);
    m_soloHighButton.removeListener(this);

    m_muteLowButton.removeListener(this);
    m_muteMidButton.removeListener(this);
    m_muteHighButton.removeListener(this);
    stopTimer();
}

void SpectrumAnalyzer::layoutBandButtons()
{
    m_soloLowButton.setButtonText("s");
    m_soloMidButton.setButtonText("s");
    m_soloHighButton.setButtonText("s");

    m_muteLowButton.setButtonText("m");
    m_muteMidButton.setButtonText("m");
    m_muteHighButton.setButtonText("m");

    auto chompLAF = juce::SharedResourcePointer<ChompLookAndFeel>();
    m_soloLowButton.setLookAndFeel(chompLAF);
    m_soloMidButton.setLookAndFeel(chompLAF);
    m_soloHighButton.setLookAndFeel(chompLAF);

    m_muteLowButton.setLookAndFeel(chompLAF);
    m_muteMidButton.setLookAndFeel(chompLAF);
    m_muteHighButton.setLookAndFeel(chompLAF);

    m_soloLowButton.addListener(this);
    m_soloMidButton.addListener(this);
    m_soloHighButton.addListener(this);

    m_muteLowButton.addListener(this);
    m_muteMidButton.addListener(this);
    m_muteHighButton.addListener(this);


    addAndMakeVisible(m_soloLowButton);
    addAndMakeVisible(m_soloMidButton);
    addAndMakeVisible(m_soloHighButton);

    addAndMakeVisible(m_muteLowButton);
    addAndMakeVisible(m_muteMidButton);
    addAndMakeVisible(m_muteHighButton);
}

void SpectrumAnalyzer::setAttachments()
{
    using namespace Params;
    const auto& paramNames = GetParams();

    m_soloLowAttachment = std::make_unique<ButtonAttachment>(m_processor.apvts, paramNames.at(Names::Solo_Low), m_soloLowButton);
    m_muteLowAttachment = std::make_unique<ButtonAttachment>(m_processor.apvts, paramNames.at(Names::Mute_Low), m_muteLowButton);

    m_soloMidAttachment = std::make_unique<ButtonAttachment>(m_processor.apvts, paramNames.at(Names::Solo_Mid), m_soloMidButton);
    m_muteMidAttachment = std::make_unique<ButtonAttachment>(m_processor.apvts, paramNames.at(Names::Mute_Mid), m_muteMidButton);

    m_soloHighAttachment = std::make_unique<ButtonAttachment>(m_processor.apvts, paramNames.at(Names::Solo_High), m_soloHighButton);
    m_muteHighAttachment = std::make_unique<ButtonAttachment>(m_processor.apvts, paramNames.at(Names::Mute_High), m_muteHighButton);


    m_lowCrossoverParam = dynamic_cast<juce::AudioParameterFloat*>(m_processor.apvts.getParameter(paramNames.at(Names::Low_Mid_Crossover_Freq)));
    m_highCrossoverParam = dynamic_cast<juce::AudioParameterFloat*>(m_processor.apvts.getParameter(paramNames.at(Names::Mid_High_Crossover_Freq)));

    m_lowClipParam = dynamic_cast<juce::AudioParameterFloat*>(m_processor.apvts.getParameter(paramNames.at(Names::Low_Clip)));
    m_midClipParam = dynamic_cast<juce::AudioParameterFloat*>(m_processor.apvts.getParameter(paramNames.at(Names::Mid_Clip)));
    m_highClipParam = dynamic_cast<juce::AudioParameterFloat*>(m_processor.apvts.getParameter(paramNames.at(Names::High_Clip)));

    m_lowGainParam = dynamic_cast<juce::AudioParameterFloat*>(m_processor.apvts.getParameter(paramNames.at(Names::Low_Gain)));
    m_midGainParam = dynamic_cast<juce::AudioParameterFloat*>(m_processor.apvts.getParameter(paramNames.at(Names::Mid_Gain)));
    m_highGainParam = dynamic_cast<juce::AudioParameterFloat*>(m_processor.apvts.getParameter(paramNames.at(Names::High_Gain)));

    for (auto& slider : m_crossoverSliders)
    {
        slider.get()->onValueChange = [this]() {
            m_processor.sendChangeMessage();
            };
    }
}

void SpectrumAnalyzer::paint(juce::Graphics& g)
{
    using namespace juce;
    g.fillAll(Colours::black);
    drawBackgroundGrid(g);
    

    auto responseArea = getAnalysisArea();

    if (shouldShowFFTAnalysis)
    {
        auto inChannelFFTPath = inPathProducer.getPath();
        inChannelFFTPath.applyTransform(AffineTransform().translation(responseArea.getX(), 0));

        g.setColour(juce::Colours::white.withAlpha(0.5f)); 
        g.fillPath(inChannelFFTPath);

        auto outChannelFFTPath = outPathProducer.getPath();
        outChannelFFTPath.applyTransform(AffineTransform().translation(responseArea.getX(), 0));

        g.setColour(juce::Colours::lightgoldenrodyellow);
        g.strokePath(outChannelFFTPath, PathStrokeType(2.f));
    }

    g.setColour(Colours::white);

    Path border;

    border.setUsingNonZeroWinding(false);

    border.addRoundedRectangle(getRenderArea(), 4);
    border.addRectangle(getLocalBounds());

    g.setColour(Colours::black);

    g.fillPath(border);


    drawCrossovers(g, responseArea);
    drawTextLabels(g);

}

int SpectrumAnalyzer::mapFrequencyToX(const float frequency)
{
    auto bounds = getAnalysisArea();
    auto normX = juce::mapFromLog10(frequency, PaxMBClip::MIN_FREQUENCY, PaxMBClip::MAX_FREQUENCY);
    return (getX() + bounds.getWidth() * normX);
}

int SpectrumAnalyzer::mapXToFrequency(const int x)
{
    auto bounds = getAnalysisArea();
    float normX = (x - getX()) / static_cast<float>(bounds.getWidth());
    float frequency = juce::mapToLog10(normX, PaxMBClip::MIN_FREQUENCY, PaxMBClip::MAX_FREQUENCY);
    return static_cast<int>(frequency);
}

void SpectrumAnalyzer::drawCrossovers(juce::Graphics& g, juce::Rectangle<int> bounds)
{
    const auto top = bounds.getY();
    const auto bottom = bounds.getBottom();
    
    const auto left = bounds.getX();
    const auto right = bounds.getRight();

    m_lowMidX = mapFrequencyToX(m_lowCrossoverParam->get());
    g.setColour(juce::Colour(188, 198, 206));
    g.drawVerticalLine(m_lowMidX, top, bottom);

    m_midHighX = mapFrequencyToX(m_highCrossoverParam->get());
    g.setColour(juce::Colour(188, 198, 206));
    g.drawVerticalLine(m_midHighX, top, bottom);

    auto highlight = bounds;
    g.setColour(juce::Colour(0x20FFFFFF));
    switch (m_processor.getBandFocus())
    {
    case BandFocus::unfocused:
        break;

        case BandFocus::Low:
            highlight.setX(0);
            highlight.setRight(m_lowMidX);
            g.fillRect(highlight);
            break;

        case BandFocus::Mid:
            highlight.setX(m_lowMidX);
            highlight.setRight(m_midHighX);
            g.fillRect(highlight);
            break;

        case BandFocus::High:
            highlight.setX(m_midHighX);
            highlight.setRight(getWidth()-1);
            g.fillRect(highlight);
            break;
    }
    m_processor.m_waveformDisplay.setBounds(highlight);
    m_processor.m_waveformDisplay.toFront(false);


    auto mapGainY = [bottom, top](float db) {

        return juce::jmap(db, -24.f, 24.f, float(bottom), float(top));
    };

    auto mapClipY = [bottom, top](float db) {

        return juce::jmap(db, -48.f, 0.f, float(bottom), float(top));
        };
    
    g.setColour(juce::Colours::green);

    g.drawHorizontalLine(mapGainY(m_lowGainParam->get()), left, m_lowMidX);
    g.drawHorizontalLine(mapGainY(m_midGainParam->get()), m_lowMidX, m_midHighX);
    g.drawHorizontalLine(mapGainY(m_highGainParam->get()), m_midHighX, right);

    g.setColour(juce::Colours::yellow);
    g.drawHorizontalLine(mapClipY(m_lowClipParam->get()), left, m_lowMidX);
    g.drawHorizontalLine(mapClipY(m_midClipParam->get()), m_lowMidX, m_midHighX);
    g.drawHorizontalLine(mapClipY(m_highClipParam->get()), m_midHighX, right);
}

void SpectrumAnalyzer::resized()
{
    auto fftBounds = getAnalysisArea().toFloat();
    auto negativeInfinity = juce::jmap(getLocalBounds().toFloat().getBottom(), fftBounds.getBottom(), fftBounds.getY(), -48.f, 0.f);

    inPathProducer.updateNegativeInfinity(negativeInfinity);
    outPathProducer.updateNegativeInfinity(negativeInfinity);

    m_lowMidX = mapFrequencyToX(m_lowCrossoverParam->get());
    m_midHighX = mapFrequencyToX(m_highCrossoverParam->get());

    m_muteLowButton.setBounds(m_lowMidX - muteOffset, getHeight() - marginY, size, size);
    m_soloLowButton.setBounds(m_lowMidX - soloOffset, getHeight() - marginY, size, size);

    m_muteMidButton.setBounds(m_midHighX - muteOffset, getHeight() - marginY, size, size);
    m_soloMidButton.setBounds(m_midHighX - soloOffset, getHeight() - marginY, size, size);

    m_muteHighButton.setBounds(getWidth() - muteOffset, getHeight() - marginY, size, size);
    m_soloHighButton.setBounds(getWidth() - soloOffset, getHeight() - marginY, size, size);

   // m_lowMidSlider.setBounds(0, 0, rotarySize, rotarySize);
   // m_midHighSlider.setBounds(m_lowMidSlider.getRight() + margin, rotaryY, rotarySize, rotarySize);

    // m_crossoverLabel.setBounds(0, m_lowMidSlider.getBottom() + margin, getWidth(), 20);

    centerBandControls();
}

void SpectrumAnalyzer::centerBandControls()
{
    deleteCrossoverSliders();

    int bandX;
    int bandY = 38;

    auto focus = m_processor.getBandFocus();

    switch (focus)
    {
    case BandFocus::Low:
    {
        bandX = bandMargin;
        break;
    }
    case BandFocus::Mid:
    {
        bandX = m_lowMidX + bandMargin;
        break;
    }
    case BandFocus::High:
    {
        bandX = m_midHighX + bandMargin;
        break;
    }

    case BandFocus::unfocused:
        return;
    }


    bandControls.setBounds(bandX, bandY, bandWidth, bandHeight);
    m_bandLabel.setBounds(bandX, bandY - 30, bandWidth, 40);

    createCrossoverSliders({ bandX, bandControls.getBottom() + bandMargin });

}

void SpectrumAnalyzer::mouseDown(const juce::MouseEvent& e)
{
    int x = e.getMouseDownX();

    if (e.mods.isLeftButtonDown())
    {
        // clicking within bounds
        if (x < m_lowMidX - 5)
        {

            m_processor.setBandFocus(BandFocus::Low);
            bandControls.setVisible(true);
            m_bandLabel.setVisible(true);
            m_bandLabel.setText("LOW", juce::NotificationType::dontSendNotification);
            resized();
            return;
        }
        else if (x > m_lowMidX + 5 && x < m_midHighX - 5)
        {
            m_processor.setBandFocus(BandFocus::Mid);
            bandControls.setVisible(true);
            m_bandLabel.setVisible(true);
            m_bandLabel.setText("MID", juce::NotificationType::dontSendNotification);

            resized();
            return;
        }
        else if (x > m_midHighX + 5)
        {
            m_processor.setBandFocus(BandFocus::High);
            bandControls.setVisible(true);
            m_bandLabel.setVisible(true);
            m_bandLabel.setText("HIGH", juce::NotificationType::dontSendNotification);

            resized();
            return;
        }

        // dragging band controls
        if (x > m_lowMidX - 5 && x < m_lowMidX + 5)
        {
            m_lowMidDragging = true;
        }
        else if (x > m_midHighX - 5 && x < m_midHighX + 5)
        {
            m_midHighDragging = true;
        }
    }
 
    else if (e.mods.isRightButtonDown())
    {
        bandControls.setVisible(false);
        m_bandLabel.setVisible(false);
        m_shouldDisplayWaveform = !m_shouldDisplayWaveform;
        m_processor.m_waveformDisplay.setVisible(m_shouldDisplayWaveform);
    }

    bandControls.m_isWaveformShowing = m_shouldDisplayWaveform;
    bandControls.repaint();
}

void SpectrumAnalyzer::mouseDrag(const juce::MouseEvent& e)
{
    auto bounds = getLocalBounds().toFloat();
    auto frequency = static_cast<float>(e.x) / bounds.getWidth();

    if (m_lowMidDragging == true)
    {
        m_lowCrossoverParam->beginChangeGesture();
        m_lowCrossoverParam->setValueNotifyingHost(frequency);
        m_lowCrossoverParam->endChangeGesture();
    }

    if (m_midHighDragging == true)
    {
        m_highCrossoverParam->beginChangeGesture();
        m_highCrossoverParam->setValueNotifyingHost(frequency);
        m_highCrossoverParam->endChangeGesture();
    }

    repaint();
    resized();
}


void SpectrumAnalyzer::mouseUp(const juce::MouseEvent& e)
{
    m_lowMidDragging = false;
    m_midHighDragging = false;
    m_frequencyDisplay.setVisible(false);
}

void SpectrumAnalyzer::mouseWheelMove(const juce::MouseEvent& event, const juce::MouseWheelDetails& wheel)
{
    const float scrollSpeed = 50.0f;
    bool isCmdDown = event.mods.isCommandDown();

    isCmdDown ? scrollGain(wheel.deltaY, scrollSpeed, nullptr) : scrollClip(wheel.deltaY, scrollSpeed, nullptr);
}

void SpectrumAnalyzer::buttonClicked(juce::Button* button)
{
    if (button == &m_soloLowButton || button == &m_muteLowButton)
    {
        m_processor.setBandFocus(BandFocus::Low);
        
    }
    else if (button == &m_soloMidButton || button == &m_muteMidButton)
    {
        m_processor.setBandFocus(BandFocus::Mid);

    }
    else if (button == &m_soloHighButton || button == &m_muteHighButton)
    {
        m_processor.setBandFocus(BandFocus::High);

    }

}

void SpectrumAnalyzer::scrollClip(float deltaY, const float scrollSpeed, juce::AudioParameterFloat* param)
{
    switch (m_processor.getBandFocus())
    {
    case BandFocus::unfocused:
        break;

    case BandFocus::Low:
        param = m_lowClipParam;
        break;

    case BandFocus::Mid:
        param = m_midClipParam;
        break;

    case BandFocus::High:
        param = m_highClipParam;
        break;
    }

    if (param)
    {
        int scrollPosition = juce::jmap(param->get(), -48.f, 0.f, -(float)getHeight(), 0.f);
        scrollPosition += static_cast<int>(deltaY * scrollSpeed);

        float newValue = juce::jmap((float)scrollPosition, -(float)getHeight(), 0.f, 0.f, 1.f);

        param->beginChangeGesture();
        param->setValueNotifyingHost(newValue);
        param->endChangeGesture();
    }
}

void SpectrumAnalyzer::scrollGain(float deltaY, const float scrollSpeed, juce::AudioParameterFloat* param)
{
    switch (m_processor.getBandFocus())
    {
    case BandFocus::unfocused:
        break;

    case BandFocus::Low:
        param = m_lowGainParam;
        break;

    case BandFocus::Mid:
        param = m_midGainParam;
        break;

    case BandFocus::High:
        param = m_highGainParam;
        break;
    }

    if (param)
    {
        int scrollPosition = juce::jmap(param->get(), -24.f, 24.f, -(float)getHeight(), 0.f);
        scrollPosition += static_cast<int>(deltaY * scrollSpeed);

        float newValue = juce::jmap((float)scrollPosition, -(float)getHeight(), 0.f, 0.f, 1.f);

        param->beginChangeGesture();
        param->setValueNotifyingHost(newValue);
        param->endChangeGesture();
    }
}



void SpectrumAnalyzer::createCrossoverSliders(const juce::Point<int> point)
{
    std::shared_ptr<RotarySlider> lowCrossover = std::make_shared<RotarySlider>();
    std::shared_ptr<RotarySlider> highCrossover = std::make_shared<RotarySlider>();
    
    using namespace Params;
    const auto& params = GetParams();
    makeAttachment(lowCrossoverAttachment, m_processor.apvts, params, Names::Low_Mid_Crossover_Freq, *lowCrossover);
    makeAttachment(highCrossoverAttachment, m_processor.apvts, params, Names::Mid_High_Crossover_Freq, *highCrossover);

    auto size = 60;
    lowCrossover->setBounds(point.getX(), point.getY(), size, size);
    highCrossover->setBounds(lowCrossover->getRight() + 5, point.getY(), size, size);

    auto chompLAF = juce::SharedResourcePointer<ChompLookAndFeel>();
    lowCrossover->setLookAndFeel(chompLAF);
    highCrossover->setLookAndFeel(chompLAF);

    addAndMakeVisible(*lowCrossover);
    addAndMakeVisible(*highCrossover);

    m_crossoverSliders.push_back(lowCrossover);
    m_crossoverSliders.push_back(highCrossover);

    
}

void SpectrumAnalyzer::deleteCrossoverSliders()
{
    lowCrossoverAttachment = nullptr;
    highCrossoverAttachment = nullptr;
    m_crossoverSliders.clear();
}

void SpectrumAnalyzer::changeListenerCallback(juce::ChangeBroadcaster* source)
{
    shouldShowFFTAnalysis = m_processor.getAnalyzerOn();
    repaint();
    resized();
}




std::vector<float> SpectrumAnalyzer::getFrequencies()
{
    return std::vector<float>
    {
        20, /*30, 40,*/ 50, 100,
            200, /*300, 400,*/ 500, 1000,
            2000, /*3000, 4000,*/ 5000, 10000,
            20000
    };
}

std::vector<float> SpectrumAnalyzer::getGains()
{
    return std::vector<float>
    {
        -24, -12, 0, 12, 24
    };
}

std::vector<float> SpectrumAnalyzer::getXs(const std::vector<float>& freqs, float left, float width)
{
    std::vector<float> xs;
    for (auto f : freqs)
    {
        auto normX = juce::mapFromLog10(f, PaxMBClip::MIN_FREQUENCY, PaxMBClip::MAX_FREQUENCY);
        xs.push_back(left + width * normX);
    }

    return xs;
}

void SpectrumAnalyzer::drawBackgroundGrid(juce::Graphics& g)
{
    using namespace juce;
    auto freqs = getFrequencies();

    auto renderArea = getAnalysisArea();
    auto left = renderArea.getX();
    auto right = renderArea.getRight();
    auto top = renderArea.getY();
    auto bottom = renderArea.getBottom();
    auto width = renderArea.getWidth();

    auto xs = getXs(freqs, left, width);

    g.setColour(Colours::dimgrey);
    for (auto x : xs)
    {
        g.drawVerticalLine(x, top, bottom);
    }

    auto gain = getGains();

    for (auto gDb : gain)
    {
        auto y = jmap(gDb, -24.f, 24.f, float(bottom), float(top));

        g.setColour(gDb == 0.f ? Colour(188, 198, 206) : Colours::darkgrey);
        g.drawHorizontalLine(y, left, right);
    }
}

void SpectrumAnalyzer::drawTextLabels(juce::Graphics& g)
{
    using namespace juce;
    g.setColour(Colours::lightgrey);
    const int fontHeight = 10;
    g.setFont(fontHeight);

    auto renderArea = getAnalysisArea();
    auto left = renderArea.getX();

    auto top = renderArea.getY();
    auto bottom = renderArea.getBottom();
    auto width = renderArea.getWidth();

    auto freqs = getFrequencies();
    auto xs = getXs(freqs, left, width);

    for (int i = 0; i < freqs.size(); ++i)
    {
        auto f = freqs[i];
        auto x = xs[i];

        bool addK = false;
        String str;
        if (f > 999.f)
        {
            addK = true;
            f /= 1000.f;
        }

        str << f;
        if (addK)
            str << "k";
        str << "Hz";

        auto textWidth = g.getCurrentFont().getStringWidth(str);

        Rectangle<int> r;

        r.setSize(textWidth, fontHeight);
        r.setCentre(x, 0);
        r.setY(1);

        g.drawFittedText(str, r, juce::Justification::centred, 1);
    }

    auto gain = getGains();

    for (auto gDb : gain)
    {
        auto y = jmap(gDb, -24.f, 24.f, float(bottom), float(top));

        String str;
        if (gDb > 0)
            str << "+";
        str << gDb;

        auto textWidth = g.getCurrentFont().getStringWidth(str);

        Rectangle<int> r;
        r.setSize(textWidth, fontHeight);
        r.setX(getWidth() - textWidth);
        r.setCentre(r.getCentreX(), y);

        g.setColour(gDb == 0.f ? Colour(188, 198, 206) : Colours::lightgrey);

        g.drawFittedText(str, r, juce::Justification::centredLeft, 1);

        str.clear();
        str << (gDb - 24.f);

        r.setX(1);
        textWidth = g.getCurrentFont().getStringWidth(str);
        r.setSize(textWidth, fontHeight);
        g.setColour(Colours::lightgrey);
        g.drawFittedText(str, r, juce::Justification::centredLeft, 1);
    }
}



void SpectrumAnalyzer::parameterValueChanged(int parameterIndex, float newValue)
{
    parametersChanged.set(true);
}




void SpectrumAnalyzer::timerCallback()
{
    if (shouldShowFFTAnalysis)
    {
        auto fftBounds = getAnalysisArea().toFloat();
        fftBounds.setBottom(getLocalBounds().getBottom());
        auto sampleRate = m_processor.getSampleRate();

        inPathProducer.process(fftBounds, sampleRate);
        outPathProducer.process(fftBounds, sampleRate);
    }

    repaint();

}

juce::Rectangle<int> SpectrumAnalyzer::getRenderArea()
{
    auto bounds = getLocalBounds();

    bounds.removeFromTop(12);
    bounds.removeFromBottom(2);
    bounds.removeFromLeft(20);
    bounds.removeFromRight(20);

    return bounds;
}


juce::Rectangle<int> SpectrumAnalyzer::getAnalysisArea()
{
    auto bounds = getRenderArea();
    bounds.removeFromTop(4);
    bounds.removeFromBottom(4);
    return bounds;
}
