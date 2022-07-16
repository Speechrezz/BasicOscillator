/*
  ==============================================================================

    SynthVoice.h
    Created: 9 Jul 2022 4:26:48pm
    Author:  Speechrezz

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "SynthSound.h"
#include "../Common/Constants.h"
#include "MyOsc.h"

class SynthVoice : public juce::SynthesiserVoice
{
public:
    bool canPlaySound(juce::SynthesiserSound* sound) override;
    void startNote(int midiNoteNumber, float velocity, juce::SynthesiserSound* sound, int currentPitchWheelPosition) override;
    void stopNote(float velocity, bool allowTailOff) override;
    void pitchWheelMoved(int newPitchWheelValue) override;
    void controllerMoved(int controllerNumber, int newControllerValue) override;
    void prepare(const juce::dsp::ProcessSpec& spec);
    void updateADSR(float attack, float decay, float sustain, float release);
    void renderNextBlock(juce::AudioBuffer<float>& outputBuffer, int startSample, int numSamples) override;

private:
    juce::dsp::ProcessSpec spec;

    juce::AudioBuffer<float> voiceBuffer;

    juce::ADSR adsr;
    juce::ADSR::Parameters adsrParams;

    xynth::Oscillator osc;
    juce::dsp::Gain<float> gain;
};