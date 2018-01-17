# Speech recognition software

### How to launch:
1. First, follow the steps descripted on [installation instruction](https://github.com/roboticslab-uc3m/speech/blob/develop/doc/speech-install.md)
2. Be sure you have a microphone connected to your computer
3. Configure the input device selecting the microphone. You can use the  default `sound settings` of Ubuntu and selecting  the input device. This requires using the Ubuntu interface. If you want to configure it remotely (by ssh), you can use `AlsaMixer` software.
In that case, run `alsamixer` on the bash,  press F6, select your Sound Card (e.g HDA Intel PCH), press F4 and select your Input Source (Front Mic). You can turn up/down the input level too.
4. Run `speechRecognition.py`. By default it takes `follow-me-english.dic` dictionary.  If you want to: know, change or add new dictionary words, you can find them in: `speech/share/speechRecognition/conf/dictionary/` directory.
5. Try to say some orders of  `follow-me`  demo using the microphone and check if 'speechRecognition' detects de words.
6. The final result in lower case comes out through a yarp port. You can connect it with the output port writting `yarp read ... /speechRecognition:o`

### How to configure it:

Once `speechRecognition.py` has started, connect it to yarp configuration dictionary port and change the language to use. 
For example, if you want to change to waiter spanish orders, put:

```bash
yarp rpc /speechRecognition/rpc:s
setDictionary waiter spanish
```

# Speech software:

### How to launch:
1. First, follow the steeps descripted on [installation instruction](https://github.com/roboticslab-uc3m/speech/blob/develop/doc/speech-install.md)
2. Be sure you have a sound speaker connected to your computer and correctly configurated (you can do it using `AlsaMixer` or directly in sound settings)
3. Run on the terminal `yarpdev --device Espeak --name /tts`. The bash will show you some information:

	```sh
	--name: /tts [/espeak]
	--voice: mb-en1 [mb-en1]
	[success] Espeak.cpp:44 setLanguage(): mb-en1
	yarp: Port /tts/rpc:s active at tcp://2.2.2.100:10061/
	[INFO]created device <Espeak>. See C++ class roboticslab::Espeak for documentation.
	yarp: Port /tts/quit active at tcp://2.2.2.100:10062/
	[INFO]device active in background...
	[INFO]device active in background...
	[INFO]device active in background...
	[INFO]device active in background...
	[INFO]device active in background...
	```

### How to configure it:

Once the speech software has started, connect it through the opened yarp port using `yarp rpc /tts/rpc:s` in other terminal. You can see all the available commands writting `help` command:

```sh
Responses:
  *** Available commands:
  setLanguage
  setSpeed
  setPitch
  getSpeed
  getPitch
  getSupportedLang
  say
  play
  pause
  stop
  help
```
### Examples of use:

```sh
>> say "Hello Teo"
>> setPitch 200
>> say "Hello Teo"
>> setLanguage mb-es1
>> say "Hola Teo"
>> setSpeed 10
>> say "Hola Teo"
>> setLanguage mb-en1
>> say "It's very cool"
```
## More examples:
To see how other programs call to `speechRecognition ` and  `Espeak` and configure it by yarp, you can see this part of the [follow-me](https://github.com/roboticslab-uc3m/follow-me/blob/develop/programs/followMeDialogueManager/FollowMeDialogueManager.cpp#L10-L100)  code.
