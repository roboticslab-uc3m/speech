# Speech recognition

## Installation and usage

This is a Python 3 application that requires the `sounddevice` package to grab live frames from a mic. Install it with:

```bash
pip install sounddevice
```

Depending on the selected backend, additional dependencies might be required (see below).

Launch the program with `--help` to see available options. You can display and select the preferred input device with `--list-devices` and `--device`, respectively (otherwise the system default will be chosen).

This application opens two YARP ports: an `<prefix>/rpc:s` port that allows to request a dictionary/model change and to mute/unmute the microphone, and a `<prefix>/result:o` port that broadcasts the transcribed text. The default prefix is `/speechRecognition`, but it can be changed with the `--prefix` option.

## PocketSphinx backend

Install the `pocketsphinx` package with:

```bash
pip install pocketsphinx
```

Then, launch the program with the `--backend pocketsphinx --dictionary xxx --language xxx` options. The dictionary and language combo relies on the adequate dictionary and model files being installed (check [share/speechRecognition](/share/speechRecognition/)). For example, to use the waiter Spanish orders dictionary, put:

```bash
speechRecognition --backend pocketsphinx --dictionary waiter --language es
```

## Vosk (Kaldi) backend

Install the `vosk` package with:

```bash
pip install vosk
```

Then, launch the program with the `--backend vosk --model xxx` options. Model files are downloaded on demand from the [Vosk website](https://alphacephei.com/vosk/models). For example, to use the ~50 MB Spanish model, put:

```bash
speechRecognition --backend vosk --model small-es-0.42
```

To list and download the desired models offline and test the Vosk engine, you can use the `vosk-transcriber` application.
