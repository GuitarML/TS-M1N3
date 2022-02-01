# TS-M1N3

[![CI](https://github.com/GuitarML/TS-M1N3/actions/workflows/cmake.yml/badge.svg)](https://github.com/GuitarML/TS-M1N3/actions/workflows/cmake.yml) [![License](https://img.shields.io/badge/License-GPL3-blue.svg)](https://opensource.org/licenses/GPL3-Clause) [![Downloads](https://img.shields.io/github/downloads/GuitarML/TS-M1N3/total)](https://somsubhra.github.io/github-release-stats/?username=GuitarML&repository=TS-M1N3&page=1&per_page=30)

![app](https://github.com/GuitarML/TS-M1N3/blob/main/resources/app.jpg)

TS-M1N3 is a guitar plugin clone of the TS-9 Tubescreamer overdrive pedal. Machine learning was used to train a model of both the drive and tone knobs for an accurate recreation of the pedal in all possible configurations. This plugin uses two conditioned parameters during model training to recreate the entire device using machine learning, as opposed to snapshot models at a particular setting. For best results, use prior to amp -> cabinet -> reverb effects to fully simulate playing an overdrive pedal through a physical amplifier. This can be done with the [NeuralPi](https://github.com/GuitarML/NeuralPi) plugin.

Check out a video demo on [Youtube](https://youtu.be/QVlmr_bECBE)

## Installing the plugin

1. Download the appropriate plugin installer [here](https://github.com/GuitarML/TS-M1N3/releases) (Windows, Mac)
2. Run the installer and follow the instructions. May need to reboot to allow your DAW to recognize the new plugin.

## Info

The [Automated-GuitarAmpModelling](https://github.com/Alec-Wright/Automated-GuitarAmpModelling) project was used to train the .json models.<br>
GuitarML maintains a [fork](https://github.com/GuitarML/Automated-GuitarAmpModelling) with a few extra helpful features, including a Colab training script and wav file processing for conditioned parameters.

The plugin uses [RTNeural](https://github.com/jatinchowdhury18/RTNeural), which is a highly optimized neural net inference engine intended for audio applications.

This plugin uses two json model files, one for 44.1kHz and one for 48kHz audio. The only difference is that the source training data is at the intended samplerate, 44.1kHz or 48kHz. Other samplerates can be used, but the sound may be distorted slightly.

For the training data, five steps for the gain and tone knobs were recorded (0.0, 0.25, 0.50, 0.75, 1.0), for a total of 25 output samples at 2 minutes each. An LSTM layer with a hidden size of 20 was used.

## Build Instructions

### Build with Cmake

```bash
# Clone the repository
$ git clone https://github.com/GuitarML/TS-M1N3.git
$ cd TS-M1N3

# initialize and set up submodules
$ git submodule update --init --recursive

# build with CMake
$ cmake -Bbuild
$ cmake --build build --config Release
```
The binaries will be located in `TS-M1N3/build/TS-M1N3_artefacts/`

### Build with Projucer 

1. Clone or download this repository.
2. Download and install [JUCE](https://juce.com/) This project uses the "Projucer" application from the JUCE website. 
3. Initialize and set up submodules
```git submodule update --init --recursive```
4. Open the TS-M1N3.jucer file and in the appropriate Exporter Header Search Path field, enter the appropriate include paths from the modules folder.
   For example:

```
   <full-path-to>/TS-M1N3/modules/json/include
   <full-path-to>/TS-M1N3/modules/RTNeural
   <full-path-to>/TS-M1N3/modules/RTNeural/modules/xsimd/include
```
5. Build TS-M1N3 from the Juce Projucer application. 

Note: Make sure to build in Release mode unless actually debugging. Debug mode will not keep up with real time playing.

### Special Thanks
Special thanks to the UAH (University of Alabama in Huntsville) [MLAMSK](https://github.com/mlamsk) Senior Design Team, whose research and hard work directly impacted the development of this plugin.
