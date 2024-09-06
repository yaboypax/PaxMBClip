#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
PaxMBClipAudioProcessorEditor::PaxMBClipAudioProcessorEditor(PaxMBClipAudioProcessor& p)
    : AudioProcessorEditor(&p), m_processor(p)

{
    if (m_processor.getSavedSize().getX() != 0 && m_processor.getSavedSize().getY() != 0)
    {
        setSize(m_processor.getSavedSize().getX(), m_processor.getSavedSize().getY());
    }
    else
    {
        setSize(MIN_WIDTH, MIN_HEIGHT);
    }

    setResizable(true, true);
    setResizeLimits(MIN_WIDTH, MIN_HEIGHT, 1600, 1000);

    controlBar.globalBypassButton.onClick = [this]()
    {
        toggleGlobalBypassState();
    };

    controlBar.settingsButton.onClick = [this]()
    {
        settingsComponent.setVisible(!settingsComponent.isVisible());
    };

    controlBar.setLookAndFeel(chompLAF);

    addAndMakeVisible(controlBar);
    addAndMakeVisible(analyzer);
    addAndMakeVisible(globalControls);

    addChildComponent(settingsComponent);

}

PaxMBClipAudioProcessorEditor::~PaxMBClipAudioProcessorEditor()
{
}

//==============================================================================
void PaxMBClipAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll(juce::Colours::black);
    updateGlobalBypassButton();
}

void PaxMBClipAudioProcessorEditor::resized()
{
    auto bounds = getLocalBounds();

    controlBar.setBounds(bounds.removeFromTop(32));
    analyzer.setBounds(0, 32, getWidth()-150, getHeight() - 32);
    //bandControls.setBounds(getWidth()-300, 32, 150, getHeight() - 32);
    globalControls.setBounds(getWidth() - 150, 32, 150, getHeight() - 32);

    bounds.reduce(200, 100);
    settingsComponent.setBounds(bounds);
    
    m_processor.setSavedSize(getWidth(), getHeight());
}

void PaxMBClipAudioProcessorEditor::toggleGlobalBypassState()
{
    bool shouldEnable = !controlBar.globalBypassButton.getToggleState();
    auto params = getBypassParams();

    for (auto* param : params)
    {
        param->beginChangeGesture();
        param->setValueNotifyingHost(!shouldEnable ? 1.f : 0.f);
        param->endChangeGesture();
    }
}

std::array<juce::AudioParameterBool*, 3> PaxMBClipAudioProcessorEditor::getBypassParams()
{
    using namespace Params;
    const auto& params = Params::GetParams();
    auto& apvts = m_processor.apvts;

    auto boolHelper = [&apvts, &params](const auto& paramName)
    {
        auto param = dynamic_cast<juce::AudioParameterBool*>(apvts.getParameter(params.at(paramName)));
        jassert(param != nullptr);
        return param;
    };

    auto* lowBypassParam = boolHelper(Names::Bypassed_Low);
    auto* midBypassParam = boolHelper(Names::Bypassed_Mid);
    auto* highBypassParam = boolHelper(Names::Bypassed_High);

    return
    {
        lowBypassParam,
        midBypassParam,
        highBypassParam
    };
}

void PaxMBClipAudioProcessorEditor::updateGlobalBypassButton()
{
    auto params = getBypassParams();
    bool allBandsBypassed = std::all_of(params.begin(), params.end(), [](const auto& param)
     {  return param->get(); });
    
    controlBar.globalBypassButton.setToggleState(allBandsBypassed, juce::NotificationType::dontSendNotification);
}