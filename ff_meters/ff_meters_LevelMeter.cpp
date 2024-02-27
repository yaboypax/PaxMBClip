/*
 ==============================================================================
 Copyright (c) 2017 Filmstro Ltd. / Foleys Finest Audio UG - Daniel Walz
 All rights reserved.

 Redistribution and use in source and binary forms, with or without modification,
 are permitted provided that the following conditions are met:
 1. Redistributions of source code must retain the above copyright notice, this
    list of conditions and the following disclaimer.
 2. Redistributions in binary form must reproduce the above copyright notice,
    this list of conditions and the following disclaimer in the documentation
    and/or other materials provided with the distribution.
 3. Neither the name of the copyright holder nor the names of its contributors
    may be used to endorse or promote products derived from this software without
    specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 OF THE POSSIBILITY OF SUCH DAMAGE.

 ==============================================================================

    ff_meters_LevelMeter.cpp
    Created: 5 Apr 2016 9:49:54am
    Author:  Daniel Walz

 ==============================================================================
*/

#include "JuceHeader.h"
using namespace juce;

//==============================================================================
LevelMeter::LevelMeter (LevelMeterLookAndFeel* lnf)
  : source      (nullptr),
    lookAndFeel (lnf),
    refreshRate (30)
{
    startTimerHz (refreshRate);
}

LevelMeter::~LevelMeter()
{
    stopTimer();
}

void LevelMeter::setMeterSource (LevelMeterSource* src)
{
    source = src;
}

void LevelMeter::setRefreshRateHz (const int newRefreshRate)
{
    refreshRate = newRefreshRate;
    startTimerHz (refreshRate);
}

void LevelMeter::setLookAndFeel (LevelMeterLookAndFeel* lnf)
{
    lookAndFeel = lnf;
}

LevelMeterLookAndFeel* LevelMeter::getLookAndFeel ()
{
    return lookAndFeel;
}

void LevelMeter::paint (Graphics& g)
{
    Graphics::ScopedSaveState saved (g);

    if (lookAndFeel) {
        lookAndFeel->drawMeters (g, getLocalBounds().toFloat (), source);
    }
}

void LevelMeter::timerCallback ()
{
    repaint();
}

void LevelMeter::mouseDown (const juce::MouseEvent &event)
{
    if (event.mods.isLeftButtonDown() && source && lookAndFeel) {
        const int numChannels = source->getNumChannels();
        for (int i=0; i < numChannels; ++i) {
            if (lookAndFeel->getClipLightBounds (getLocalBounds().toFloat(),
                                                 numChannels, i)
                .contains (event.getPosition().toFloat())) {
                source->clearClipFlag (i);
                return;
            }
        }
    }
}

void LevelMeter::mouseDoubleClick (const juce::MouseEvent& event)
{
    if (source && lookAndFeel) {
        const int numChannels = source->getNumChannels();
        for (int i=0; i < numChannels; ++i) {
            if (lookAndFeel->getClipLightBounds (getLocalBounds().toFloat(),
                                                 numChannels, i)
                .contains (event.getPosition().toFloat())) {
                source->clearAllClipFlags();
                return;
            }
        }
    }
}
