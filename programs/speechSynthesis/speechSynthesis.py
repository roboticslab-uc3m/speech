#!/usr/bin/env python3

import argparse
import roboticslab_speech
import sounddevice as sd
import yarp

from abc import ABC, abstractmethod

class SynthesizerFactory(ABC):
    @abstractmethod
    def create(self, player):
        pass

class PiperSynthesizerFactory(SynthesizerFactory):
    def __init__(self, model, use_cuda, rf):
        self.model = model
        self.use_cuda = use_cuda
        self.rf = rf

    def create(self, player):
        return PiperSynthesizer(player, self.model, self.use_cuda, self.rf)

class SpeechSynthesizer(roboticslab_speech.SpeechSynthesis):
    def __init__(self, player):
        super().__init__()
        self._player = player

    def say(self, text):
        print('saying: %s' % text)
        self._player.set_generator(self._get_generator(text))
        return True

    def play(self):
        self._player.resume()
        print('resumed')
        return True

    def pause(self):
        self._player.pause()
        print('paused')
        return True

    def stop(self):
        self._player.clear_generator()
        print('stopped')
        return True

    def checkSayDone(self):
        return not self._player.is_playing()

    def setSpeed(self, speed):
        return super().setSpeed(speed)

    def setPitch(self, pitch):
        return super().setPitch(pitch)

    def getSpeed(self):
        return super().getSpeed()

    def getPitch(self):
        return super().getPitch()

    @abstractmethod
    def get_sample_rate(self):
        pass

    @abstractmethod
    def _get_generator(self, text):
        pass

class PiperSynthesizer(SpeechSynthesizer):
    def __init__(self, player, model, use_cuda, rf):
        super().__init__(player)
        self._rf = rf
        self._use_cuda = use_cuda
        self._voice = self._load_model(model)

    def _load_model(self, model):
        from piper import PiperVoice

        if not model.endswith('.onnx'):
            model += '.onnx'

        path = rf.findFileByName(model)
        return PiperVoice.load(path, use_cuda=self._use_cuda)

    def get_sample_rate(self):
        return self._voice.config.sample_rate

    def _get_generator(self, text):
        return self._voice.synthesize_stream_raw(text)

    def setLanguage(self, language):
        try:
            self._voice = self._load_model(language)
            print('switched language to %s' % language)
            return True
        except IOError as e:
            print(e)
            return False

    def getSupportedLangs(self):
        from pathlib import Path
        path = Path(self._rf.findFile('from'))
        return yarp.SVector([f.stem for f in path.parent.iterdir() if f.is_file() and f.name.endswith('.onnx')])

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
parser.add_argument('--model', type=str, required=True, help='model, e.g. es_ES-davefx-medium')
parser.add_argument('--cuda', action='store_true', help='use ONNX CUDA execution provider (requires onnxruntime-gpu)')
parser.add_argument('--prefix', '-p', type=str, default='/speechSynthesis', help='YARP port prefix')
parser.add_argument('--context', type=str, default='speechSynthesis', help='YARP context directory')
parser.add_argument('--from', type=str, dest='ini', default='speechSynthesis.ini', help='YARP configuration (.ini) file')
args = parser.parse_args(remaining)

yarp.Network.init()

rf = yarp.ResourceFinder()
rf.setDefaultContext(args.context)
rf.setDefaultConfigFile(args.ini)

if args.backend == 'piper':
    synthesizer_factory = PiperSynthesizerFactory(args.model, args.cuda, rf)
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

class CallbackPlayer:
    def __init__(self):
        self._generator = None
        self._is_paused = False
        self._buffer = bytearray()

    def set_generator(self, generator):
        self._generator = generator
        self._is_paused = False
        self._buffer.clear()

    def clear_generator(self):
        self._generator = None
        self._is_paused = False
        self._buffer.clear()

    def pause(self):
        self._is_paused = True

    def resume(self):
        self._is_paused = False

    def is_playing(self):
        return self._generator is not None and not self._is_paused

    def callback(self, outdata, frames, time, status):
        # https://stackoverflow.com/a/62609827

        if self.is_playing():
            try:
                if len(self._buffer) == 0:
                    self._buffer = bytearray(next(self._generator))

                if len(outdata) >= len(self._buffer):
                    outdata[:len(self._buffer)] = self._buffer
                    outdata[len(self._buffer):] = b'\x00' * (len(outdata) - len(self._buffer))
                    self._buffer.clear()
                else:
                    # print('len(outdata) < len(raw)')
                    outdata[:] = self._buffer[:len(outdata)]
                    self._buffer = self._buffer[len(outdata):]

                return
            except StopIteration:
                self.clear_generator()

        outdata[:] = b'\x00' * len(outdata)

try:
    player = CallbackPlayer()
    synthesizer = synthesizer_factory.create(player)

    with sd.RawOutputStream(samplerate=synthesizer.get_sample_rate(),
                            blocksize=1024,
                            device=args.device,
                            dtype='int16',
                            channels=1,
                            callback=player.callback) as stream:
        synthesizer.yarp().attachAsServer(rpc)

        while True:
            import time
            time.sleep(0.1)
except KeyboardInterrupt:
    rpc.interrupt()
    rpc.close()
    parser.exit(0)
