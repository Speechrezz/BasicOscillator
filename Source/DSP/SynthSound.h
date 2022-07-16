/*
  ==============================================================================

    SynthSound.h
    Created: 9 Jul 2022 4:27:04pm
    Author:  thesp

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

class SynthSound : public juce::SynthesiserSound
{
public:
    inline bool appliesToNote(int midiNoteNumber) override
    {
        return true;
    }
    inline bool appliesToChannel(int midiChannel) override
    {
        return true;
    }
};