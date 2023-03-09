# Speech synthesis

## Installation

Through pip:

```bash
pip3 install mycroft-mimic3-tts
```

Alternatively, install from sources: https://github.com/MycroftAI/mimic3

## Download voice models

All voice data is located in a separate repository: https://github.com/MycroftAI/mimic3-voices

To manually issue the download of all Spanish voices, run:

```bash
mimic3-download 'es_ES/*'
```

In case the process gets stuck, download and unpack the files into `${HOME}/.local/share/mycroft/mimic3/voices`. However, you'll probably need to download the *generator.onnx* file separately (via GitHub) since it is handled by Git LFS.

## Troubleshooting

Try this:

```bash
mimic3 --voice es_ES/m-ailabs#tux "hola, me llamo teo y tengo 10 años"
```

To enable GPU acceleration, run `pip3 install onnxruntime-gpu` and issue the `mimic3` command with `--cuda`. The `speechSynthesis` app also accepts this parameter.
