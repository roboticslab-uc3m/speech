#!/usr/bin/env python3

import argparse
import queue
import roboticslab_speech
import sounddevice as sd
import threading
import yarp

from abc import ABC, abstractmethod
from piper import PiperVoice # TODO: hide import
from piper.download import find_voice # TODO: hide import

class SpeechSynthesizer(roboticslab_speech.SpeechSynthesis):
    def __init__(self):
        super().__init__()
        self.event = threading.Event()
        self._generator = None

    @abstractmethod
    def get_sample_rate(self):
        pass

    def say(self, text):
        self._generator = self._make_generator(text)
        self.event.set()
        return True

    @abstractmethod
    def _make_generator(self, text):
        pass

    def get_generator(self):
        return self._generator

class PiperSynthesizer(SpeechSynthesizer):
    def __init__(self, model, rf):
        super().__init__()
        self.model = model
        self.rf = rf
        self.voice = PiperVoice.load(self.model, use_cuda=False) # TODO: cuda

    def get_sample_rate(self):
        return self.voice.config.sample_rate

    def _make_generator(self, text):
        return self.voice.synthesize_stream_raw(text)

    def play(self):
        pass

    def pause(self):
        pass

    def stop(self):
        pass

    def checkSayDone(self):
        pass

    def setLanguage(self, language):
        pass

    def setSpeed(self, speed):
        pass

    def setPitch(self, pitch):
        pass

    def getSpeed(self):
        pass

    def getPitch(self):
        pass

    def getSupportedLangs(self):
        pass

def int_or_str(text):
    """Helper function for argument parsing."""
    try:
        return int(text)
    except ValueError:
        return text

BACKENDS = ['piper']

parser = argparse.ArgumentParser(description='YARP service that transforms text into live audio output', add_help=False)
parser.add_argument('--list-devices', action='store_true', help='list available audio devices and exit')
parser.add_argument('--list-backends', action='store_true', help='list available TTS backends and exit')
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
parser.add_argument('--model', type=str, help='model, e.g. follow-me')
parser.add_argument('--cuda', action='store_true', help='Use Onnx CUDA execution provider (requires onnxruntime-gpu)')
parser.add_argument('--prefix', '-p', type=str, default='/speechSynthesis', help='YARP port prefix')
parser.add_argument('--context', type=str, default='speechSynthesis', help='YARP context directory')
parser.add_argument('--from', type=str, dest='ini', default='speechSynthesis.ini', help='YARP configuration (.ini) file')
args = parser.parse_args(remaining)

yarp.Network.init()

rf = yarp.ResourceFinder()
rf.setDefaultContext(args.context)
rf.setDefaultConfigFile(args.ini)

if args.backend == 'piper':
    if args.model is None:
        print('Model must be specified for Piper')
        raise SystemExit

    synthesizer = PiperSynthesizer(args.model, rf)
else:
    print('Backend not available, must be one of: %s' % ', '.join(BACKENDS))
    raise SystemExit

if not yarp.Network.checkNetwork():
    print('No YARP network available')
    raise SystemExit

rpc = yarp.RpcServer()

if not rpc.open(args.prefix + '/rpc:s'):
    print('Unable to open RPC port')
    raise SystemExit

try:
    with sd.RawOutputStream(samplerate=synthesizer.get_sample_rate(),
                            blocksize=1024,
                            device=args.device,
                            dtype='int16',
                            channels=1) as stream:
        synthesizer.yarp().attachAsServer(rpc)

        while True:
            print('Waiting for text to synthesize...')
            synthesizer.event.wait()
            print('Synthesizing...')

            for frames in synthesizer.get_generator():
                print(stream.write_available)
                stream.write(frames)

            print('Done synthesizing')
            synthesizer.event.clear()
except KeyboardInterrupt:
    rpc.interrupt()
    rpc.close()
    parser.exit(0)
