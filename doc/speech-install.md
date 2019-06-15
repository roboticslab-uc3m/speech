# Installation from Source Code

### Install the Software dependencies

- [Install CMake 3.5+](https://github.com/roboticslab-uc3m/installation-guides/blob/master/install-cmake.md/)
- [Install YCM 0.10+](https://github.com/roboticslab-uc3m/installation-guides/blob/master/install-ycm.md/)
- [Install YARP 3.1+](https://github.com/roboticslab-uc3m/installation-guides/blob/master/install-yarp.md/)
with [Python bindings](https://github.com/roboticslab-uc3m/installation-guides/blob/master/install-yarp.md#install-python-bindings)
- [Install eSpeak with MBROLA Voices](https://github.com/roboticslab-uc3m/installation-guides/blob/master/install-espeak-mbrola.md/)
- [Install gstreamer with pocketsphinx](https://github.com/roboticslab-uc3m/installation-guides/blob/master/install-gstreamer-pocketsphinx.md/)
- [Install color-debug](https://github.com/roboticslab-uc3m/color-debug)

### Install the Software on Ubuntu (working on all tested versions)

Our software integrates the previous dependencies. Note that you will be prompted for your password upon using `sudo` a couple of times:

```bash
pip install --user pyalsaaudio # Used in speechRecognition.py
cd  # go home
mkdir -p repos; cd repos  # create $HOME/repos if it doesn't exist; then, enter it
git clone https://github.com/roboticslab-uc3m/speech.git  # Download speech software from the repository
cd speech; mkdir build; cd build; cmake ..  # Configure the speech software
make -j$(nproc) # Compile
sudo make install  # Install :-)
```

You may want to (from the same build dir):
```bash
echo "export ROBOTICSLAB_SPEECH_DIR=`pwd`" >> ~/.bashrc
```
For additional SPEECH options use `ccmake` instead of `cmake`.

### Troubleshooting installation
For `pip install --user pyalsaaudio`, some users have had to:
```
sudo apt install python-gi # requirement on some systems for pyalsaaudio
sudo apt install libasound2-dev # requirement on some systems for pyalsaaudio
```

### Troubleshooting selecting default soundcard
This is a way set default sound output card using PulseAudio (not ALSA).

1. Check list of Sources and Sinks
```bash
$ pactl list short sources
0	alsa_input.usb-PrimeSense_PrimeSense_Device-01.analog-stereo	module-alsa-card.c	s16le 2ch 44100Hz
1	alsa_output.usb-C-Media_Electronics_Inc._USB_PnP_Sound_Device-00.analog-stereo.monitor	module-alsa-card.c	s16le 2ch 48000Hz	
2	alsa_input.usb-C-Media_Electronics_Inc._USB_PnP_Sound_Device-00.analog-mono	module-alsa-card.c	s16le 1ch 44100Hz	
3	alsa_output.pci-0000_00_1f.3.hdmi-stereo.monitor	module-alsa-card.c	s16le 2ch 44100Hz	

$ pactl list short sinks
0	alsa_output.usb-C-Media_Electronics_Inc._USB_PnP_Sound_Device-00.analog-stereo	module-alsa-card.c	s16le 2ch 48000Hz	
1	alsa_output.pci-0000_00_1f.3.hdmi-stereo	module-alsa-card.c	s16le 2ch 44100Hz	
```

2. Set defaults (you can add these lines in the `.profile` file to save this configuration by default):
```bash
pactl set-default-source alsa_output.usb-C-Media_Electronics_Inc._USB_PnP_Sound_Device-00.analog-stereo.monitor
pactl set-default-sink alsa_output.usb-C-Media_Electronics_Inc._USB_PnP_Sound_Device-00.analog-stereo
```

