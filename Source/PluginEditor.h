/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class BasicOscillatorAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    BasicOscillatorAudioProcessorEditor (BasicOscillatorAudioProcessor&);
    ~BasicOscillatorAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    BasicOscillatorAudioProcessor& audioProcessor;

    using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;

    juce::Slider attackSlider;
    juce::Slider decaySlider;
    juce::Slider sustainSlider;
    juce::Slider releaseSlider;
    std::unique_ptr<SliderAttachment> attackAttach;
    std::unique_ptr<SliderAttachment> decayAttach;
    std::unique_ptr<SliderAttachment> sustainAttach;
    std::unique_ptr<SliderAttachment> releaseAttach;

    juce::ComboBox oscSelector;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> oscAttach;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BasicOscillatorAudioProcessorEditor)
};
