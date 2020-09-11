# Speech

Text To Speech (TTS) and Automatic Speech Recognition (ASR).

Link to Doxygen generated documentation: http://robots.uc3m.es/dox-speech

## Installation

Installation instructions for installing from source can be found [here](doc/speech-install.md).

## Usage

Information of how to launch or configure it:
1. [Speech recognition](programs/speechRecognition/README.md)
2. [Espeak](libraries/YarpPlugins/Espeak/README.md)

Read the [development manual](https://github.com/roboticslab-uc3m/speech/blob/develop/doc/speech-development-manual.md) if you want to build a new language model

##### More examples:
To see how other programs call to `speechRecognition ` and  `Espeak` and configure it by yarp, you can see this part of the [follow-me](https://github.com/roboticslab-uc3m/follow-me/blob/develop/programs/followMeDialogueManager/FollowMeDialogueManager.cpp#L10-L100)  code.

## Contributing

#### Posting Issues

1. Read [CONTRIBUTING.md](CONTRIBUTING.md)
2. [Post an issue / Feature request / Specific documentation request](https://github.com/roboticslab-uc3m/speech/issues)

#### Fork & Pull Request

1. [Fork the repository](https://github.com/roboticslab-uc3m/speech/fork)
2. Create your feature branch (`git checkout -b my-new-feature`) off the `develop` branch, following the [GitFlow git workflow](https://www.atlassian.com/git/tutorials/comparing-workflows/gitflow-workflow)
3. Commit your changes
4. Push to the branch (`git push origin my-new-feature`)
5. Create a new Pull Request

## Status

[![Build Status (Linux)](https://travis-ci.com/roboticslab-uc3m/speech.svg?branch=develop)](https://travis-ci.com/roboticslab-uc3m/speech)

[![Coverage Status](https://coveralls.io/repos/roboticslab-uc3m/speech/badge.svg)](https://coveralls.io/r/roboticslab-uc3m/speech)

[![Issues](https://img.shields.io/github/issues/roboticslab-uc3m/speech.svg?label=Issues)](https://github.com/roboticslab-uc3m/speech/issues)

## Similar and Related Projects
- http://kaldi-asr.org/
- [robotology/yarp.js/examples/speech_recognition](https://github.com/robotology/yarp.js/tree/master/examples/speech_recognition)
