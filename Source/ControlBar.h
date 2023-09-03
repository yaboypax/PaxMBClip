/*
  ==============================================================================

    ControlBar.h
    Created: 2 Sep 2023 7:39:03pm
    Author:  Paxton Fleming

  ==============================================================================
*/

#pragma once
#include "JuceHeader.h"
#include "LookAndFeel.h"

class ControlBar : public juce::Component
{
public:
    
    ControlBar();
    void resized() override;
    
    AnalyzerButton analyzerButton;
    PowerButton globalBypassButton;
    
private:
    LookAndFeel lnf;
};