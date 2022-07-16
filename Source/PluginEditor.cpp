/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
BasicOscillatorAudioProcessorEditor::BasicOscillatorAudioProcessorEditor (BasicOscillatorAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    setSize(500, 400);
    using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;

    attackAttach = std::make_unique<SliderAttachment>(audioProcessor.treeState, ATTACK_ID, attackSlider);
    decayAttach = std::make_unique<SliderAttachment>(audioProcessor.treeState, DECAY_ID, decaySlider);
    sustainAttach = std::make_unique<SliderAttachment>(audioProcessor.treeState, SUSTAIN_ID, sustainSlider);
    releaseAttach = std::make_unique<SliderAttachment>(audioProcessor.treeState, RELEASE_ID, releaseSlider);

    oscAttach = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(audioProcessor.treeState, OSC_ID, oscSelector);

    attackSlider.setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
    decaySlider.setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
    sustainSlider.setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
    releaseSlider.setSliderStyle(juce::Slider::SliderStyle::LinearVertical);

    addAndMakeVisible(oscSelector);
    addAndMakeVisible(attackSlider);
    addAndMakeVisible(decaySlider);
    addAndMakeVisible(sustainSlider);
    addAndMakeVisible(releaseSlider);
}

BasicOscillatorAudioProcessorEditor::~BasicOscillatorAudioProcessorEditor()
{
}

//==============================================================================
void BasicOscillatorAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
}

void BasicOscillatorAudioProcessorEditor::resized()
{
    auto bounds = getLocalBounds().reduced(40);
    oscSelector.setBounds(bounds.removeFromTop(50).reduced(40, 10));

    bounds.removeFromRight(50);
    const auto width = bounds.getWidth() / 4;
    attackSlider.setBounds(bounds.removeFromLeft(width));
    decaySlider.setBounds(bounds.removeFromLeft(width));
    sustainSlider.setBounds(bounds.removeFromLeft(width));
    releaseSlider.setBounds(bounds.removeFromLeft(width));
}
