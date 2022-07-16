/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
BasicOscillatorAudioProcessor::BasicOscillatorAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ),
    treeState(*this, nullptr, "PARAMETER", { createParameterLayout() })
#endif
{
    for (int i = 0; i < 8; i++)
        synth.addVoice(new SynthVoice());
    synth.addSound(new SynthSound());
}

BasicOscillatorAudioProcessor::~BasicOscillatorAudioProcessor()
{
}

juce::AudioProcessorValueTreeState::ParameterLayout BasicOscillatorAudioProcessor::createParameterLayout()
{
    // Add parameters to the paramater tree. Allows for saving/loading and automating of paramaters

    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    // Combobox: switch oscillators
    auto oscParam = std::make_unique<juce::AudioParameterChoice>(OSC_ID, "Oscillator", oscVoices, 0);
    params.push_back(std::move(oscParam));

    // ADSR for synth voice
    auto attackParam = std::make_unique<juce::AudioParameterFloat>(ATTACK_ID, "Attack", 0.001f, 1.f, 0.001f);
    params.push_back(std::move(attackParam));
    auto decayParam = std::make_unique<juce::AudioParameterFloat>(DECAY_ID, "Decay", 0.01f, 1.f, 0.4f);
    params.push_back(std::move(decayParam));
    auto sustainParam = std::make_unique<juce::AudioParameterFloat>(SUSTAIN_ID, "Sustain", 0.f, 1.f, 0.4f);
    params.push_back(std::move(sustainParam));
    auto releaseParam = std::make_unique<juce::AudioParameterFloat>(RELEASE_ID, "Release", 0.01f, 3.f, 0.04f);
    params.push_back(std::move(releaseParam));

    return { params.begin(), params.end() };
}

//==============================================================================
const juce::String BasicOscillatorAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool BasicOscillatorAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool BasicOscillatorAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool BasicOscillatorAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double BasicOscillatorAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int BasicOscillatorAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int BasicOscillatorAudioProcessor::getCurrentProgram()
{
    return 0;
}

void BasicOscillatorAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String BasicOscillatorAudioProcessor::getProgramName (int index)
{
    return {};
}

void BasicOscillatorAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void BasicOscillatorAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.numChannels = getNumOutputChannels();
    spec.maximumBlockSize = samplesPerBlock;

    synth.setCurrentPlaybackSampleRate(sampleRate);

    for (int i = 0; i < synth.getNumVoices(); i++)
    {
        if (auto voice = dynamic_cast<SynthVoice*>(synth.getVoice(i)))
        {
            voice->prepare(spec);
        }
    }
}

void BasicOscillatorAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool BasicOscillatorAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void BasicOscillatorAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear(i, 0, buffer.getNumSamples());

    juce::dsp::AudioBlock<float> audioBlock(buffer);
    juce::dsp::ProcessContextReplacing<float> context(audioBlock);

    for (int i = 0; i < synth.getNumVoices(); ++i)
    {
        if (auto voice = dynamic_cast<SynthVoice*>(synth.getVoice(i)))
        {
            // update voice
            const auto attack = treeState.getRawParameterValue(ATTACK_ID)->load();
            const auto decay = treeState.getRawParameterValue(DECAY_ID)->load();
            const auto sustain = treeState.getRawParameterValue(SUSTAIN_ID)->load();
            const auto release = treeState.getRawParameterValue(RELEASE_ID)->load();
            voice->updateADSR(attack, decay, sustain, release);
        }
    }


    synth.renderNextBlock(buffer, midiMessages, 0, buffer.getNumSamples());
}

//==============================================================================
bool BasicOscillatorAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* BasicOscillatorAudioProcessor::createEditor()
{
    return new BasicOscillatorAudioProcessorEditor (*this);
}

//==============================================================================
void BasicOscillatorAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    auto state = treeState.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void BasicOscillatorAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));

    // 'If' statements are for error checking
    if (xmlState != nullptr) {
        if (xmlState->hasTagName(treeState.state.getType())) {
            treeState.replaceState(juce::ValueTree::fromXml(*xmlState));
        }
    }
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new BasicOscillatorAudioProcessor();
}
