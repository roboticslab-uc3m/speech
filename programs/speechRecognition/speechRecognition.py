#!/usr/bin/env python3

##
# @ingroup speech-programs
# @defgroup speechRecognition speechRecognition.py
# @brief Provides basic ASR capabilities.
#
# First we must run a YARP name server if it is not running in our current namespace:
#
# \verbatim
# [on terminal 1] yarp server
# \endverbatim
#
# Now launch the program (use --help for a list of options):
#
# @verbatim
# [on terminal 2] speechRecognition --backend vosk --model es-0.42
# @endverbatim
#
# You can launch a 3rd terminal to read what is published via YARP port:
#
# @verbatim
# [on terminal 3] yarp read ...  /speechRecognition/result:o
# @endverbatim
#
# or you can send commands to configure the speech recognition app. To know how to do that, use "help":
# @verbatim
# [on terminal 4] yarp rpc /speechRecognition/rpc:s
# [on terminal 4] >>help
# @endverbatim
#
# For instance, you can load the 20k words Spanish dictionary for the PocketSphinx backend with:
# @verbatim
# [on terminal 4] >>setDictionary 20k es
# @endverbatim

# borrowed from https://github.com/alphacep/vosk-api/blob/12f29a3/python/example/test_microphone.py
# and https://github.com/cmusphinx/pocketsphinx/blob/b16c631/examples/live.py

import argparse
import json
import queue
import roboticslab_speech
import sounddevice as sd
import yarp

from abc import ABC, abstractmethod
from pocketsphinx import Endpointer, Decoder
from vosk import Model, KaldiRecognizer

class ResponderFactory(ABC):
    @abstractmethod
    def create(self, stream):
        pass

class PocketSphinxResponderFactory(ResponderFactory):
    def __init__(self, device, dictionary, language, rf):
        self.device = device
        self.dictionary = dictionary
        self.language = language
        self.rf = rf

    def create(self, stream):
        return PocketSphinxSpeechRecognitionResponder(stream, self.device, self.dictionary, self.language, self.rf)

class VoskResponderFactory(ResponderFactory):
    def __init__(self, device, model):
        self.device = device
        self.model = model

    def create(self, stream):
        return VoskSpeechRecognitionResponder(stream, self.device, self.model)

class SpeechRecognitionResponder(roboticslab_speech.SpeechRecognitionIDL):
    def __init__(self, stream, device):
        super().__init__()
        self.stream = stream
        device_info = sd.query_devices(device, 'input')
        # soundfile expects an int, sounddevice provides a float:
        self.sample_rate = int(device_info['default_samplerate'])

    def setDictionary(self, dictionary, language):
        self.stream.abort()

        try:
            return self._setDictionaryInternal(dictionary, language)
        finally:
            self.stream.start()

    @abstractmethod
    def _setDictionaryInternal(self, dictionary, language):
        pass

    def muteMicrophone(self):
        print('Muting microphone')
        self.stream.abort()
        return True

    def unmuteMicrophone(self):
        print('Unmuting microphone')
        self.stream.start()
        return True

    @abstractmethod
    def transcribe(self, frame):
        pass

class PocketSphinxSpeechRecognitionResponder(SpeechRecognitionResponder):
    def __init__(self, stream, device, dictionary, language, rf):
        super().__init__(stream, device)
        self.rf = rf

        if not self._setDictionaryInternal(dictionary, language):
            raise Exception('Unable to load dictionary')

    def _setDictionaryInternal(self, dictionary, language):
        print('Setting dictionary to %s (language: %s)' % (dictionary, language))

        lm = self.rf.findFileByName('dictionary/%s-%s.lm' % (dictionary, language))
        dic = self.rf.findFileByName('dictionary/%s-%s.dic' % (dictionary, language))
        model = self.rf.findFileByName('model/%s' % language)

        if not lm or not dic or not model:
            print('Unable to find dictionary')
            return False

        self.lm, self.dic, self.model = lm, dic, model
        self.ep = Endpointer(sample_rate=self.sample_rate)

        self.decoder = Decoder(
            hmm=self.model,
            lm=self.lm,
            dict=self.dic,
            samprate=self.ep.sample_rate,
            #loglevel='INFO' # might be a bit too noisy
        )

        return True

    def transcribe(self, frame):
        prev_in_speech = self.ep.in_speech
        speech = self.ep.process(frame)

        if speech is not None:
            if not prev_in_speech:
                self.decoder.start_utt()

            self.decoder.process_raw(speech)
            hyp = self.decoder.hyp()

            if not self.ep.in_speech:
                self.decoder.end_utt()
                return False, self.decoder.hyp().hypstr
            elif hyp is not None:
                # partial result
                return True, hyp.hypstr
        return False, None

class VoskSpeechRecognitionResponder(SpeechRecognitionResponder):
    def __init__(self, stream, device, model):
        super().__init__(stream, device)

        if model is None:
            self.model = Model(lang='en-us')
        else:
            self.model = Model(model_name='vosk-model-' + model)

        self.rec = KaldiRecognizer(self.model, self.sample_rate)

    def _setDictionaryInternal(self, dictionary, language):
        try:
            if dictionary is not None and str(dictionary):
                print('Setting dictionary to %s' % dictionary)
                self.model = Model(model_name='vosk-model-' + dictionary)
            elif language is not None and str(language):
                print('Setting language to %s' % language)
                self.model = Model(lang=language)
            else:
                print('No dictionary or language specified')
                return False
        except SystemExit:
            print('Dictionary or language not available')
            return False

        self.rec = KaldiRecognizer(self.model, self.sample_rate)
        return True

    def transcribe(self, frame):
        if self.rec.AcceptWaveform(frame):
            return (False, json.loads(self.rec.Result())['text'])
        else:
            return (True, json.loads(self.rec.PartialResult())['partial'])

def int_or_str(text):
    """Helper function for argument parsing."""
    try:
        return int(text)
    except ValueError:
        return text

BACKENDS = ['pocketsphinx', 'vosk']

parser = argparse.ArgumentParser(description='YARP service that transforms live microphone input into transcribed text', add_help=False)
parser.add_argument('--list-devices', action='store_true', help='list available audio devices and exit')
parser.add_argument('--list-backends', action='store_true', help='list available ASR backends and exit')
args, remaining = parser.parse_known_args()

if args.list_devices:
    print(sd.query_devices())
    raise SystemExit
elif args.list_backends:
    print('\n'.join(BACKENDS))
    raise SystemExit

parser = argparse.ArgumentParser(description=parser.description, formatter_class=argparse.ArgumentDefaultsHelpFormatter, parents=[parser])
parser.add_argument('--backend', '-b', type=str, required=True, help='ASR backend engine')
parser.add_argument('--device', '-d', type=int_or_str, help='input device (numeric ID or substring)')
parser.add_argument('--dictionary', type=str, help='(only --backend pocketsphinx) dictionary, e.g. follow-me')
parser.add_argument('--language', type=str, help='(only --backend pocketsphinx) language, e.g. en-us')
parser.add_argument('--model', type=str, help='(only --backend vosk) model, e.g. es-0.42')
parser.add_argument('--prefix', '-p', type=str, default='/speechRecognition', help='YARP port prefix')
parser.add_argument('--context', type=str, default='speechRecognition', help='YARP context directory')
parser.add_argument('--from', type=str, dest='ini', default='speechRecognition.ini', help='YARP configuration (.ini) file')
args = parser.parse_args(remaining)

yarp.Network.init()

rf = yarp.ResourceFinder()
rf.setDefaultContext(args.context)
rf.setDefaultConfigFile(args.ini)

if args.backend == 'pocketsphinx':
    if args.dictionary is None or args.language is None:
        print('Dictionary and language must be specified for PocketSphinx')
        raise SystemExit

    responder_factory = PocketSphinxResponderFactory(args.device, args.dictionary, args.language, rf)
elif args.backend == 'vosk':
    if args.model is None:
        print('Model must be specified for Vosk')
        raise SystemExit

    responder_factory = VoskResponderFactory(args.device, args.model)
else:
    print('Backend not available, must be one of: %s' % ', '.join(BACKENDS))
    raise SystemExit

if not yarp.Network.checkNetwork():
    print('No YARP network available')
    raise SystemExit

asrPort = yarp.BufferedPortBottle()
configPort = yarp.RpcServer()

if not asrPort.open(args.prefix + '/result:o'):
    print('Unable to open output port')
    raise SystemExit

if not configPort.open(args.prefix + '/rpc:s'):
    print('Unable to open RPC port')
    raise SystemExit

try:
    q = queue.Queue()

    with sd.RawInputStream(blocksize=8000, #int(2880 / 2), #FIXME: hardcoded for pocketpshinx, vosk used to have 8000 here
                           device=args.device,
                           dtype='int16',
                           channels=1,
                           callback=lambda indata, frames, time, status: q.put(bytes(indata))) as stream:
        responder = responder_factory.create(stream)
        responder.yarp().attachAsServer(configPort)

        while True:
            frame = q.get()
            isPartial, transcription = responder.transcribe(frame)

            if transcription:
                if not isPartial:
                    print('result: %s' % transcription)
                    b = asrPort.prepare()
                    b.clear()
                    b.addString(transcription)
                    asrPort.write()
                else:
                    print('partial: %s' % transcription)
except KeyboardInterrupt:
    asrPort.interrupt()
    asrPort.close()
    configPort.interrupt()
    configPort.close()
    parser.exit(0)
