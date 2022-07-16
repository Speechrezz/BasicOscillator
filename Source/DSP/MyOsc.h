/*
  ==============================================================================

    MyOsc.h
    Created: 14 Jul 2022 5:55:10pm
    Author:  Speechrezz

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "Wavetable.h"

namespace xynth
{

class Oscillator
{
public:
    Oscillator() = default;
    ~Oscillator();

    void prepare(const juce::dsp::ProcessSpec& spec);
    void setFrequency(float freq);

    void process(juce::dsp::ProcessContextReplacing<float> context);

private:
    void updateAngleDelta(float freq);
    float currentSampleRate = 0.f, currentAngle = 0.f, angleDelta = 0.f;

    Wavetable wavetable;

    juce::dsp::ProcessSpec spec;
};

}