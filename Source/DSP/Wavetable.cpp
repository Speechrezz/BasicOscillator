/*
  ==============================================================================

    Wavetable.cpp
    Created: 14 Jul 2022 8:51:19pm
    Author:  Speechrezz

  ==============================================================================
*/

#include "Wavetable.h"

namespace xynth
{

Wavetable::Wavetable() : fft(FFT_ORDER)
{
    // Initialize wavetable
    const auto inc = 2.f / float(FFT_SIZE - 1);
    const float pd = juce::MathConstants<float>::twoPi / float(FFT_SIZE);
    for (int i = 0; i < FFT_SIZE; ++i)
    {
        //fftTime[i] = std::sin(float(i) * pd); // sine
        //fftTime[i] = i < FFT_SIZE / 2 ? 1.f : -1.f; //square
        fftTime[i] = float(i) * inc - 1.f; // saw
    }

    // Perform forward fft
    fft.perform(fftTime, fftFreq, false);
}
Wavetable::~Wavetable()
{
}

void Wavetable::setFrequency(float freq)
{
    float numHarmonics = std::floor(NYQUIST / freq);

    // Forward FFT
    std::copy(std::begin(fftFreq), std::end(fftFreq), std::begin(fftFreqCopy));

    // Remove harmonics above Nyquist
    for (int i = FFT_SIZE - 1; i >= numHarmonics; --i)
        fftFreqCopy[i] = std::complex<float>(0.f, 0.f);

    // Inverse FFT
    fft.perform(fftFreqCopy, fftTime, true);

    // Convert back to time domain
    for (int i = 0; i < FFT_SIZE; ++i)
        wavetable[i + 1] = fftTime[i].real();

    // Adjust wavetable
    wavetable[0] = wavetable[FFT_SIZE];
    wavetable[FFT_SIZE + 1] = wavetable[1];
    wavetable[FFT_SIZE + 2] = wavetable[2];
}

float Wavetable::getSample(float currentAngle)
{
    int i0 = int(currentAngle);
    float offset = currentAngle - std::floor(currentAngle);
    return hermite(wavetable[i0], wavetable[i0 + 1], wavetable[i0 + 2], wavetable[i0 + 3], offset);
}

float Wavetable::hermite(float v0, float v1, float v2, float v3, float offset)
{
    float slope0 = (v2 - v0) * 0.5f;
    float slope1 = (v3 - v1) * 0.5f;
    float v = v1 - v2;
    float w = slope0 + v;
    float a = w + v + slope1;
    float b_neg = w + a;
    float stage1 = a * offset - b_neg;
    float stage2 = stage1 * offset + slope0;
    return stage2 * offset + v1;
}

}