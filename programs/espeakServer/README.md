# Speech software

### How to launch
1. First, follow the steps described in the [installation instructions](doc/speech-install.md).
2. Be sure you have a sound speaker connected to your computer and correctly configured (you can do that using `alsamixer` or directly in the sound settings).
3. Run `espeakServer --name /tts` on the terminal.

### How to configure it

Once the speech software has started, connect it through the opened yarp port using `yarp rpc /tts/rpc:s` in another terminal. You can see all the available commands writing the `help` command:

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
  checkSayDone
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
