# Speech recognition software

### How to launch
1. First, follow the steps described on [installation instructions](doc/speech-install.md)
2. Be sure you have a microphone connected to your computer.
3. Configure the input device selecting the microphone. You can use the  default `sound settings` of Ubuntu and select the input device. This requires using the Ubuntu interface. If you want to configure it remotely (by ssh), you can use the `alsamixer` software.
In that case, run `alsamixer` on the bash,  press `F6`, select your Sound Card (e.g HDA Intel PCH), press `F4` and select your `Input Source (Front Mic)`. You can turn the input level up/down too.
4. Run `speechRecognition.py`. By default it uses the `follow-me-english.dic` dictionary.  If you want to: know, change or add new dictionary words, you can find them in: `speech/share/speechRecognition/conf/dictionary/` directory.
5. Try to say some orders of  `follow-me`  demo using the microphone and check if `speechRecognition` detects de words.
6. The final result in lower case comes out through a yarp port. You can read from the output port writing `yarp read ... /speechRecognition:o`.

### How to configure it

Once `speechRecognition.py` has started, connect it to the yarp configuration dictionary port and change the language to use. 
For example, if you want to change to waiter Spanish orders, put:

```bash
yarp rpc /speechRecognition/rpc:s
setDictionary waiter spanish
```
