# Installation from Source Code

### Install the Software dependencies

- [Install CMake 3.5+](https://github.com/roboticslab-uc3m/installation-guides/blob/master/install-cmake.md/)
- [Install YCM 0.10+](https://github.com/roboticslab-uc3m/installation-guides/blob/master/install-ycm.md/)
- [Install YARP 2.3.70+](https://github.com/roboticslab-uc3m/installation-guides/blob/master/install-yarp.md/)
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
