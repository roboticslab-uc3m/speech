# Vosk Transcription

## Requirements

Depends on:

- [Vosk](https://github.com/roboticslab-uc3m/installation-guides/blob/master/docs/install-vosk.md)

## Download models

All models are stored in Alpha Cephei's servers: <https://alphacephei.com/vosk/models>

By default, `VoskTranscription` assumes `--context VoskTranscription`, i.e. it will spawn a `ResourceFinder` instance and look for a `VoskTranscription.ini` placed in a `VoskTranscription/` directory following the [YARP data directory specification](https://www.yarp.it/latest/yarp_data_dirs.html). These default context and configuration file can be modified via command line arguments, although it shouldn't be necessary. Models need to be downloaded manually into the sibling directory of the .ini configuration file.

It is advised to import the `VoskTranscription` context after installing the speech repository:

```bash
yarp-config context --import VoskTranscription
```

This command will copy the installed context into a writable user-local path such as `$HOME/.local/share/yarp/contexts/VoskTranscription`. Change into this directory, download the models manually and place them here.
