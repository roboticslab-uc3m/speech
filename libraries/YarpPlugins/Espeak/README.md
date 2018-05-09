# Speech software

### How to launch
1. First, follow the steps described on [installation instruction](https://github.com/roboticslab-uc3m/speech/blob/develop/doc/speech-install.md)
2. Be sure you have a sound speaker connected to your computer and correctly configured (you can do it using `AlsaMixer` or directly in sound settings)
3. Run on the terminal `yarpdev --device Espeak --name /tts`. The bash will show you some information:

	```bash
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

### How to configure it

Once the speech software has started, connect it through the opened yarp port using `yarp rpc /tts/rpc:s` in another terminal. You can see all the available commands writting the `help` command:

```bash
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
### Examples of use

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
