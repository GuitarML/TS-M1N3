# TS-M1N3

[![CI](https://github.com/GuitarML/TS-M1N3/actions/workflows/cmake.yml/badge.svg)](https://github.com/GuitarML/TS-M1N3/actions/workflows/cmake.yml) [![License: GPL v3](https://img.shields.io/badge/License-GPLv3-brightgreen.svg)](https://www.gnu.org/licenses/gpl-3.0) [![Downloads](https://img.shields.io/github/downloads/GuitarML/TS-M1N3/total)](https://somsubhra.github.io/github-release-stats/?username=GuitarML&repository=TS-M1N3&page=1&per_page=30)

![app](https://github.com/GuitarML/TS-M1N3/blob/main/resources/app.jpg)

TS-M1N3 is a guitar plugin clone of the TS-9 Tubescreamer overdrive pedal. Machine learning was used to train a model of both the drive and tone knobs for an accurate recreation of the pedal in all possible configurations. This plugin uses two conditioned parameters during model training to recreate the entire device using machine learning, as opposed to snapshot models at a particular setting. For best results, use prior to amp -> cabinet -> reverb effects to fully simulate playing an overdrive pedal through a physical amplifier. This can be done with the [NeuralPi](https://github.com/GuitarML/NeuralPi) plugin.

Check out a video demo on [Youtube](https://youtu.be/QVlmr_bECBE)

## Installing the plugin

1. Download the appropriate plugin installer [here](https://github.com/GuitarML/TS-M1N3/releases) (Windows, Mac, Linux)
2. Run the installer and follow the instructions. May need to reboot to allow your DAW to recognize the new plugin.

## Info

The [Automated-GuitarAmpModelling](https://github.com/Alec-Wright/Automated-GuitarAmpModelling) project was used to train the .json models.<br>
GuitarML maintains a [fork](https://github.com/GuitarML/Automated-GuitarAmpModelling) with a few extra helpful features, including a Colab training script and wav file processing for conditioned parameters.

The plugin uses [RTNeural](https://github.com/jatinchowdhury18/RTNeural), which is a highly optimized neural net inference engine intended for audio applications.

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

### Special Thanks
Special thanks to the UAH (University of Alabama in Huntsville) [MLAMSK](https://github.com/mlamsk) Senior Design Team, whose research and hard work directly impacted the development of this plugin.
