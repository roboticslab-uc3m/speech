# Piper Synthesizer

## Requirements

Depends on:

- [Piper](https://github.com/roboticslab-uc3m/installation-guides/blob/master/docs/install-piper.md)

## Download voice models

All voice data is stored in Hugging Face: <https://huggingface.co/rhasspy/piper-voices>

By default, `PiperSynthesizer` assumes `--context PiperSynthesizer`, i.e. it will spawn a `ResourceFinder` instance and look for a `PiperSynthesizer.ini` placed in a `PiperSynthesizer/` directory following the [YARP data directory specification](https://www.yarp.it/latest/yarp_data_dirs.html). These default context and configuration file can be modified via command line arguments, although it shouldn't be necessary. Voice models need to be downloaded manually into the sibling directory of the .ini configuration file.

It is advised to import the `PiperSynthesizer` context after installing the speech repository:

```bash
yarp-config context --import PiperSynthesizer
```

This command will copy the installed context into a writable user-local path such as `$HOME/.local/share/yarp/contexts/PiperSynthesizer`. Change into this directory, download the voice models manually from the Hugging Face repository and place them here.
