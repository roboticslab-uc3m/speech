# Installation from Source Code

### Install the Software dependencies:

- [Install CMake](https://github.com/roboticslab-uc3m/installation-guides/blob/master/install-cmake.md)
- [Install YARP with Python bindings](https://github.com/roboticslab-uc3m/installation-guides/blob/master/install-yarp.md)
- [Install MBROLA VOICES](https://github.com/roboticslab-uc3m/installation-guides/blob/master/install-mbrola.md)
- [Install Speech Recognition](https://github.com/roboticslab-uc3m/installation-guides/blob/master/install-speech-recognition.md)

### Install the Software on Ubuntu (working on all tested versions)

Our software integrates the previous dependencies. Note that you will be prompted for your password upon using `sudo` a couple of times:

```bash
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
