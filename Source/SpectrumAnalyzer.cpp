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

SpectrumAnalyzer::SpectrumAnalyzer(PaxMBClipAudioProcessor& p) :
    m_processor(p),
    leftPathProducer(m_processor.leftChannelFifo),
    rightPathProducer(m_processor.rightChannelFifo)
{
    const auto& params = m_processor.getParameters();
    for (auto param : params)
    {
        param->addListener(this);
    }

    using namespace Params;
    const auto& paramNames = GetParams();

    m_lowMidXoverParam = dynamic_cast<juce::AudioParameterFloat*>(m_processor.apvts.getParameter(paramNames.at(Names::Low_Mid_Crossover_Freq)));
    m_midHighXoverParam = dynamic_cast<juce::AudioParameterFloat*>(m_processor.apvts.getParameter(paramNames.at(Names::Mid_High_Crossover_Freq)));

    m_lowClipParam = dynamic_cast<juce::AudioParameterFloat*>(m_processor.apvts.getParameter(paramNames.at(Names::Low_Clip)));
    m_midClipParam = dynamic_cast<juce::AudioParameterFloat*>(m_processor.apvts.getParameter(paramNames.at(Names::Mid_Clip)));
    m_highClipParam = dynamic_cast<juce::AudioParameterFloat*>(m_processor.apvts.getParameter(paramNames.at(Names::High_Clip)));

    startTimerHz(60);
}

SpectrumAnalyzer::~SpectrumAnalyzer()
{
    const auto& params = m_processor.getParameters();
    for (auto param : params)
    {
        param->removeListener(this);
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
        auto leftChannelFFTPath = leftPathProducer.getPath();
        leftChannelFFTPath.applyTransform(AffineTransform().translation(responseArea.getX(), 0));

        g.setColour(Colour(97u, 18u, 167u)); //purple-
        g.strokePath(leftChannelFFTPath, PathStrokeType(1.f));

        auto rightChannelFFTPath = rightPathProducer.getPath();
        rightChannelFFTPath.applyTransform(AffineTransform().translation(responseArea.getX(), 0));

        g.setColour(Colour(215u, 201u, 134u));
        g.strokePath(rightChannelFFTPath, PathStrokeType(1.f));
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

void SpectrumAnalyzer::drawCrossovers(juce::Graphics& g, juce::Rectangle<int> bounds)
{
    const auto top = bounds.getY()+20;
    const auto bottom = bounds.getBottom()-20;
    
    const auto left = bounds.getX();
    const auto right = bounds.getRight();

    auto mapX = [left = bounds.getX(), width = bounds.getWidth()](float frequency) {
        auto normX = juce::mapFromLog10(frequency, PaxMBClip::MIN_FREQUENCY, PaxMBClip::MAX_FREQUENCY);
        return (left + width * normX);
    };

    m_lowMidX = mapX(m_lowMidXoverParam->get());
    g.setColour(juce::Colour(188, 198, 206));
    g.drawVerticalLine(m_lowMidX, top, bottom);

    m_midHighX = mapX(m_midHighXoverParam->get());
    g.setColour(juce::Colour(188, 198, 206));
    g.drawVerticalLine(m_midHighX, top, bottom);

    auto mapY = [bottom, top](float db) {

        return juce::jmap(db, -24.f, 24.f, float(bottom), float(top));
    };
    
    g.setColour(juce::Colours::yellow);
    g.drawHorizontalLine(mapY(m_lowClipParam->get()), left, m_lowMidX);
    g.drawHorizontalLine(mapY(m_midClipParam->get()), m_lowMidX, m_midHighX);
    g.drawHorizontalLine(mapY(m_highClipParam->get()), m_midHighX, right);
}

void SpectrumAnalyzer::mouseDown(const juce::MouseEvent& e)
{
    auto x = e.getMouseDownX();

    if (x < m_lowMidX)
    {
        m_processor.setBandFocus(BandFocus::Low);
    }
    else if (x > m_lowMidX && x < m_midHighX)
    {
        m_processor.setBandFocus(BandFocus::Mid);
    }
    else if (x > m_midHighX)
    {
        m_processor.setBandFocus(BandFocus::High);
    }
    else return;
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

void SpectrumAnalyzer::resized()
{
    auto fftBounds = getAnalysisArea().toFloat();
    auto negativeInfinity = juce::jmap(getLocalBounds().toFloat().getBottom(), fftBounds.getBottom(), fftBounds.getY(), -48.f, 0.f);

    leftPathProducer.updateNegativeInfinity(negativeInfinity);
    rightPathProducer.updateNegativeInfinity(negativeInfinity);
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

        leftPathProducer.process(fftBounds, sampleRate);
        rightPathProducer.process(fftBounds, sampleRate);
    }

    if (parametersChanged.compareAndSetBool(false, true))
    {

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