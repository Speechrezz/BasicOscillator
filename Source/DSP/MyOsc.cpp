/*
  ==============================================================================

    MyOsc.cpp
    Created: 14 Jul 2022 5:55:10pm
    Author:  Speechrezz

  ==============================================================================
*/

#include "MyOsc.h"

namespace xynth
{

Oscillator::~Oscillator()
{
}

void Oscillator::prepare(const juce::dsp::ProcessSpec& _spec)
{
    spec = _spec;
    currentSampleRate = spec.sampleRate;
}
void Oscillator::setFrequency(float freq)
{
    updateAngleDelta(freq);
    wavetable.setFrequency(freq);
}

void Oscillator::process(juce::dsp::ProcessContextReplacing<float> context)
{
    auto left = context.getOutputBlock().getChannelPointer(0);
    auto right = context.getOutputBlock().getChannelPointer(1);
    const auto numSamples = context.getOutputBlock().getNumSamples();

    for (int i = 0; i < numSamples; ++i)
    {
        float currentSample = wavetable.getSample(currentAngle);
        currentAngle += angleDelta;
        currentAngle = fmod(currentAngle, float(FFT_SIZE));
        left[i] = currentSample;
        right[i] = currentSample;
    }
}

void Oscillator::updateAngleDelta(float freq)
{
    float cyclesPerSample = float(freq) / currentSampleRate;
    angleDelta = cyclesPerSample * float(FFT_SIZE);
}


}