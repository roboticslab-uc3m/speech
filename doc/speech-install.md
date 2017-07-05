# Installation from Source Code

### Install the Software dependencies:

- [Install CMake](https://github.com/roboticslab-uc3m/installation-guides/blob/master/install-cmake.md)
- [Install YARP](https://github.com/roboticslab-uc3m/installation-guides/blob/master/install-yarp.md)
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

**Note:** `speechRecognition.py` is written in python and uses YARP. YARP is written in C++, so to use all the features of YARP in a program written in a different language, you have to use SWIG and configure YARP for this point:
```bash
sudo apt-get install swig
cd
cd repos/yarp/build # go to your YARP build path
cmake .. -DYARP_COMPILE_BINDINGS=ON -DCREATE_PYTHON=ON #configuring YARP to be used in python programs
make -j$(nproc) # Compile
sudo make install  # Install :-)
```


