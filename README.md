# Speech

Text To Speech (TTS) and Automatic Speech Recognition (ASR).

Link to Doxygen generated documentation: https://robots.uc3m.es/speech/

## Installation

Installation instructions for installing from source can be found [here](doc/speech-install.md).

## Usage

Information of how to launch or configure it:
1. [Speech recognition](programs/speechRecognition/README.md)
2. [Espeak](programs/espeakServer/README.md)

Read the [development manual](doc/speech-development-manual.md) if you want to build a new language model

##### More examples:
To see how other programs call to `speechRecognition` and  `Espeak` and configure it by yarp, check the [teo-follow-me](https://github.com/roboticslab-uc3m/teo-follow-me/blob/master/programs/followMeDialogueManager/FollowMeDialogueManager.cpp) demonstration.

## Contributing

#### Posting Issues

1. Read [CONTRIBUTING.md](CONTRIBUTING.md)
2. [Post an issue / Feature request / Specific documentation request](https://github.com/roboticslab-uc3m/speech/issues)

#### Fork & Pull Request

1. [Fork the repository](https://github.com/roboticslab-uc3m/speech/fork)
2. Create your feature branch (`git checkout -b my-new-feature`) off the `master` branch, following the [Forking Git workflow](https://www.atlassian.com/git/tutorials/comparing-workflows/forking-workflow)
3. Commit your changes
4. Push to the branch (`git push origin my-new-feature`)
5. Create a new Pull Request

## Citation

If you found this project useful, please consider citing the following works:

- [chat application](programs/chatCompletion/) and related modules

Bartek Łukawski, Juan G. Victores, Carlos Balaguer, and Alberto Jardón. Interaction with a humanoid robot through a conversational interface using DeepSeek. In *Simposio CEA de Robótica, Bioingeniería, Visión Artificial y Automática Marina*, 2025.

```bibtex
@inproceedings{lukawski2025rbvm,
    author    = {{\L}ukawski, Bartek and Victores, Juan G. and Balaguer, Carlos and Jardón, Alberto},
    title     = {Interaction with a humanoid robot through a conversational interface using DeepSeek},
    booktitle = {Simposio CEA de Robótica, Bioingeniería, Visión Artificial y Automática Marina},
    year      = {2025},
}
```

## Status

[![Continuous Integration](https://github.com/roboticslab-uc3m/speech/actions/workflows/ci.yml/badge.svg)](https://github.com/roboticslab-uc3m/speech/actions/workflows/ci.yml)

[![Issues](https://img.shields.io/github/issues/roboticslab-uc3m/speech.svg?label=Issues)](https://github.com/roboticslab-uc3m/speech/issues)

## Similar and Related Projects
- http://kaldi-asr.org/
- [robotology/yarp.js/examples/speech_recognition](https://github.com/robotology/yarp.js/tree/master/examples/speech_recognition)
