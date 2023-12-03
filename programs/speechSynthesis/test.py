#!/usr/bin/env python3

import argparse
import queue
import roboticslab_speech
import sounddevice as sd
import yarp

from abc import ABC, abstractmethod
from piper import PiperVoice # TODO: hide import
from piper.download import find_voice # TODO: hide import

class SynthesizerFactory(ABC):
    @abstractmethod
    def create(self, stream, callback):
        pass

class PiperSynthesizerFactory(SynthesizerFactory):
    def __init__(self, device, model, rf):
        self.device = device
        self.model = model
        self.rf = rf

    def create(self, stream, callback):
        return PiperSynthesizer(stream, callback, self.device, self.model, self.rf)

class SpeechSynthesizer(roboticslab_speech.SpeechSynthesis):
    def __init__(self, stream, callback, device):
        super().__init__()
        self.stream = stream
        self.callback = callback
        device_info = sd.query_devices(device, 'input')
        # soundfile expects an int, sounddevice provides a float:
        self.sample_rate = int(device_info['default_samplerate'])
        self.text_queue = queue.Queue(maxsize=1)

    def say(self, text):
        self.callback.set_generator(self._get_generator(text))
        return True

    @abstractmethod
    def synthesize(self, text):
        pass

    @abstractmethod
    def _get_generator(self, text):
        pass

    @abstractmethod
    def sd_callback(self, outdata, frames, time, status):
        pass

class PiperSynthesizer(SpeechSynthesizer):
    def __init__(self, stream, callback, device, model, rf):
        super().__init__(stream, callback, device)
        self.model = model
        self.rf = rf
        self.voice = PiperVoice.load(self.model, use_cuda=False) # TODO: cuda

    def _get_generator(self, text):
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

    synthesizer_factory = PiperSynthesizerFactory(args.device, args.model, rf)
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

class Callback:
    def __init__(self):
        self._generator = None
        self._queued_generator = None

    def set_generator(self, generator):
        self._generator = generator

    def callback(self, outdata, frames, time, status):
        # https://stackoverflow.com/a/62609827

        if self._generator is not None:
            try:
                raw = next(self._generator)

                print('len(raw): %d, len(outdata): %d' % (len(raw), len(outdata)))

                if len(outdata) > len(raw):
                    outdata[:len(raw)] = raw
                    outdata[len(raw):] = b'\x00' * (len(outdata) - len(raw))
                    print('a')
                elif len(outdata) < len(raw):
                    outdata[:] = raw[:len(outdata)]
                    self._queued_generator = self._generator
                    self._generator = iter([raw[len(outdata):]])
                    print('b')
                else:
                    outdata[:] = raw
                    print('c')

                return
            except StopIteration:
                if self._queued_generator is not None:
                    self._generator = self._queued_generator
                    self._queued_generator = None

        outdata[:] = b'\x00' * len(outdata)

try:
    cb = Callback()

    with sd.RawOutputStream(samplerate=22050,
                            blocksize=2048,
                            device=args.device,
                            dtype='int16',
                            channels=1,
                            callback=cb.callback) as stream:
        synthesizer = synthesizer_factory.create(stream, cb)
        synthesizer.yarp().attachAsServer(rpc)

        while True:
            import time
            time.sleep(0.1)
except KeyboardInterrupt:
    rpc.interrupt()
    rpc.close()
    parser.exit(0)
