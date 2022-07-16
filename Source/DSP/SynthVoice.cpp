/*
  ==============================================================================

    SynthVoice.cpp
    Created: 9 Jul 2022 4:26:48pm
    Author:  Speechrezz

  ==============================================================================
*/

#include "SynthVoice.h"

bool SynthVoice::canPlaySound(juce::SynthesiserSound* sound)
{
    jassert(sound != nullptr);
    return dynamic_cast<juce::SynthesiserSound*>(sound) != nullptr;
}

void SynthVoice::startNote(int midiNoteNumber, float velocity, juce::SynthesiserSound* sound, int currentPitchWheelPosition)
{
    const float freq = juce::MidiMessage::getMidiNoteInHertz(midiNoteNumber);
    osc.setFrequency(freq);
    adsr.noteOn();
}

void SynthVoice::stopNote(float velocity, bool allowTailOff)
{
    adsr.noteOff();

    if (!allowTailOff || !adsr.isActive())
        clearCurrentNote();
}

void SynthVoice::pitchWheelMoved(int newPitchWheelValue)
{
}

void SynthVoice::controllerMoved(int controllerNumber, int newControllerValue)
{
}

void SynthVoice::prepare(const juce::dsp::ProcessSpec& _spec)
{
    spec = _spec;
    osc.prepare(spec);

    gain.prepare(spec);
    gain.setGainLinear(0.1f);

    adsr.setSampleRate(spec.sampleRate);
    adsr.setParameters(adsrParams);

    voiceBuffer.setSize(spec.numChannels, spec.maximumBlockSize);
}

void SynthVoice::updateADSR(float attack, float decay, float sustain, float release)
{
    adsrParams.attack  = attack;
    adsrParams.decay   = decay;
    adsrParams.sustain = sustain;
    adsrParams.release = release;

    adsr.setParameters(adsrParams);
}

void SynthVoice::renderNextBlock(juce::AudioBuffer<float>& buffer, int startSample, int numSamples)
{
    if (!isVoiceActive() && !adsr.isActive()) return;
    voiceBuffer.clear();

    // Need to shrink down the audioBlock to be numSamples in size, or else the oscillator
    // will continue to render past numSamples, resulting in discontinuities.
    // renderNextBlock() can be called in the middle of the processBlock, so startSample and numSamples
    // may not be 0.
    auto audioBlock = juce::dsp::AudioBlock<float>(voiceBuffer).getSubBlock(startSample, numSamples);
    juce::dsp::ProcessContextReplacing<float> context(audioBlock);
    osc.process(context);
    gain.process(context);

    adsr.applyEnvelopeToBuffer(voiceBuffer, startSample, numSamples);

    for (int ch = 0; ch < buffer.getNumChannels(); ch++)
        buffer.addFrom(ch, startSample, voiceBuffer, ch, startSample, numSamples);

    if (!adsr.isActive())
        clearCurrentNote();
}