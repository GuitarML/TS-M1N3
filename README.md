# TS-M1N3

## MOD Branch
IMPORTANT: This is the MOD branch for TS-M1N3, which does not include GUI code or resources. 
Because the mod toolchain uses gcc7, you will need to modify the json module code to reference "experimental/filesystem" instead of just "filesystem", or else it won't compile.

IMPORTANT: The instrctions used here are specific to MODEP (Patchbox OS) running on a Raspberry Pi3, and testing using [Pi-Stomp](https://github.com/TreeFallSound/pi-stomp) hardware. You will need to modify your build environment to match your target system in steps 2 and 3 below. Note that Patchbox OS is based on Raspian which is a 32-bit OS, not 64-bit.

Note: This is a prototype build process. Hoping to simplify this in the future. 

Required additional modifications after cloning TS-M1N3 repo and updating submodules:
1. Remove the JUCE module so that cmake can find the MOD toolchain version of JUCE. 
2. Update json module code to reference "experimental/filesystem" instead of just "filesystem". There are several locations in the c++ code that this must be done.

Note: The internal samplerate conversion code for the neural net model has been removed since MOD devices use a fixed samplerate of 48kHz. 
Note: References to the plugin editor have been removed, and gui resources removed from the cmake files. This is required to compile for MOD.

## To compile LV2 for Mod devices / Rasberry Pi based devices (ARM architecture):
1. Clone the latest [mod-plugin-builder](https://github.com/moddevices/mod-plugin-builder)
2. Create the mod environment for building the plugin (can take about an hour to complete):
```
cd mod-plugin-builder
./bootstrap modduo-static juce  

# Replace "modduo-static" in above command with appropriate target as referenced in mod-plugin-builder Readme.
```
Note: The "modduo-static" target uses the modduo build options but updated with GCC7.5 instead of GCC4.9. This is required to compile the TS-M1N3 code.

3. Modify the modduo-static ".config" file for Raspberry Pi3 with optimizations:
```
# In ~/mod-workdir/modduo-static/.config
# Modify the .config file with these values at the proper location (search for BR2_TARGET_OPTIMIZATION):

BR2_TARGET_OPTIMIZATION="-mcpu=cortex-a53 -mtune=cortex-a53 -mfpu=neon-fp-armv8 -mfloat-abi=hard -mvectorize-with-neon-quad -ffast-math -fno-finite-math-only -fprefetch-loop-arrays -funroll-loops -funsafe-loop-optimizations -D__MOD_DEVICES__ -D_MOD_DEVICE_DUO -static-libstdc++ -Wl,-Ofast -Wl,--as-needed -Wl,--strip-all"
BR2_TARGET_LDFLAGS="-static-libstdc++ -Wl,-Ofast -Wl,--as-needed -Wl,--strip-all"
```
Note: The modified flags will be ```-mcpu, -mtune, -mfpu, and -O```. This is done to target the Rpi3 specific architecture and optimize for SIMD instructions. 

4. After the above step finishes, in the ```mod-plugin-builder``` directory, run the following to import the mod toolchain:
```. local.env modduox-static```
5. In the same terminal as step 3, cd into the mod branch of the TS-M1N3 repo and run these commands to build (after making required modifications mentioned above):
```
cmake -Bbuild {JUCE_TOOLCHAIN_FILE} -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
```
If all goes well, you should have a LV2 folder with a compiled .so. 
6. To generate the TTL files, compile and run the lv2-ttl-generator tool and run
```
./lv2-ttl-generator <path-to-lv2-so>
```
Note: Since the LV2 is for ARM architecture, the lv2-ttl-generator must be compiled and run using ARM as the target
Note: lv2-ttl-generator code is located in the [DPF repo](https://github.com/DISTRHO/DPF/tree/main/utils/lv2-ttl-generator)
Note: The method I used was to scp the DPF repo onto the Raspberry Pi 3, and the use "make" to build the lv2-ttl-generator in the local environment. 
      I then used scp to copy the TS-M1N3.lv2 onto the Raspberry Pi 3, and ran ./lv2-ttl-generator TS-M1N3.lv2/TS-M1N3.so to generate the ttl files. The ttl files will
      be generated in the same directory as the tool, so you have to move these into the TS-M1N3.lv2 directory.
      I then scp'd the TS-M1N3.lv2 onto my Windows machine to use the MOD-SDK to generate the html GUI.

7. With your completed LV2 plugin package, follow the instructions on the MOD-SDK wiki for creating an html gui for use in the virtual pedalboard.
[MOD-SDK wiki](https://wiki.moddevices.com/wiki/MOD_SDK)

8. After the html gui files have been created for your LV2, deploy the finalized LV2 to your device. If you have an official MOD device, you can deploy using the MOD-SDK (MOD-SDK deploy does not work for MODEP/PatchboxOS).
This procedure can be done for modep devices such as the Pi-Stomp (patchbox OS):
    A. Turn on your device.
    B. Run these steps (or similar) to get your lv2 package to your device at the proper location.
```
scp -r TS-M1N3.lv2 patch@patchbox.local:/home/patch/
ssh patch@pathcbox.local
sudo cp -r /home/patch/TS-M1N3.lv2 /usr/modep/lv2/
```
    C. Restart your device.

9. If all goes well, you should be able to use the virtual pedalboard to drag and drop the TS-M1N3 into your pedalboard. CPU usage was noted at 33% during testing (that's alot), it's a work-in-progress to optimize further. 

Note: For more information, see these MOD wiki pages:
https://wiki.moddevices.com/wiki/How_To_Build_and_Deploy_LV2_Plugin_to_MOD_Duo
https://wiki.moddevices.com/wiki/Developers
