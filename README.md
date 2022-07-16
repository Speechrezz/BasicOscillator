# Basic Oscillator

Implements a band-limited oscillator using Fourier Transforms in order to prevent aliasing due to harmonics going above the Nyquist limit.

AKA it sounds very clean :)

This code is far from as optimized as it could be, but it does work and it's not too heavy on the cpu. Furthermore, more improvements can be made to make this oscillator more flexible (like ability to change wavetables, pitch bends, etc.).

## Usage

Feel free to use this oscillator in your own projects. In order to use, simply copy `MyOsc.cpp`, `MyOsc.h`, `Wavetable.cpp` and `Wavetable.h` into your own project.

## Credit

This implementation is based on Matt Tytel's talk: https://youtu.be/qlinVx60778

## License

MIT