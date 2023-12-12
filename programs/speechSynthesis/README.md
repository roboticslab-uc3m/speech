# Speech synthesis

## Installation

Note that **Python 3.9+ is required**. Through pip:

```bash
pip install piper-tts
```

Alternatively, install from sources: <https://github.com/rhasspy/piper>

## Download voice models

All voice data is stored in Hugging Face: <https://huggingface.co/rhasspy/piper-voices>

By default, `speechSynthesis` assumes `--context speechSynthesis --from speechSynthesis.ini`, i.e. it will spawn a `ResourceFinder` instance and look for a `speechSynthesis.ini` placed in a `speechSynthesis/` directory following the [YARP data directory specification](https://www.yarp.it/latest/yarp_data_dirs.html). These default context and configuration file can be modified via command line arguments, although it shouldn't be necessary. Voice models need to be downloaded either manually or via `piper` into the sibling directory of the .ini configuration file.

It is advised to import the `speechSynthesis` context after installating the speech repository:

```bash
yarp-config context --import speechSynthesis
```

This command will copy the installed context into a writable user-local path such as `$HOME/.local/share/yarp/contexts/speechSynthesis`. Change into this directory and run `piper` (see examples below) to automatically download the voice models, or download them manually from the Hugging Face repository and place them here.

The following command will output nothing, it simply downloads the model (if available in Hugging Face) and blocks the terminal since it expects input from stdin (kill it with Ctrl+C after the download is complete):

```bash
piper --model es_ES-davefx-medium
```

## Troubleshooting

Try this (requires `pip install aplay`):

```bash
echo "hola, me llamo teo y tengo 10 años" | piper --model es_ES-davefx-medium --output-raw | aplay -r 22050 -f S16_LE -t raw -
```

Alternatively, keep the application open while reading from stdin:

```bash
piper --model es_ES-davefx-medium --output-raw | aplay -r 22050 -f S16_LE -t raw -
```

To enable GPU acceleration, run `pip install onnxruntime-gpu` and issue the `piper` command with `--cuda`. The `speechSynthesis` app also accepts this parameter.
