#!/usr/bin/env python3

# adapted from https://github.com/MycroftAI/mimic3/blob/be72c18/mimic3_tts/__main__.py

import argparse
import queue
import signal
import shlex
import shutil
import subprocess
import tempfile
import threading
import time

import mimic3_tts
import yarp
import roboticslab_speech

PLAY_PROGRAMS = ['paplay', 'play -q', 'aplay -q']

class TextToSpeechResponder(roboticslab_speech.TextToSpeechIDL):
    def __init__(self, engine):
        super().__init__()
        self.engine = engine
        self.is_playing = False
        self.p = None
        self.result_queue = queue.Queue(maxsize=5)
        self.result_thread = threading.Thread(target=self._process_result, daemon=True)
        self.result_thread.start()

    def setLanguage(self, language):
        if language.startswith('#'):
            # same voice, different speaker
            self.engine.speaker = language[1:]
        else:
            # different voice
            self.engine.voice = language

        if self.engine.voice not in list(self.getSupportedLangs()):
            print('Voice not available: %s' % self.engine.voice)
            return False
        else:
            self.engine.preload_voice(self.engine.voice)
            print('Loaded voice: %s (speaker: %s)' % (self.engine.voice, self.engine.speaker or 'default'))
            return True

    def setSpeed(self, speed):
        self.engine.rate = float(speed) / 100
        return True

    def setPitch(self, pitch):
        return super().setPitch(pitch)

    def getSpeed(self):
        return int(self.engine.rate * 100)

    def getPitch(self):
        return super().getPitch()

    def getSupportedLangs(self):
        all_voices = sorted(list(self.engine.get_voices()), key=lambda v: v.key)
        local_voices = filter(lambda v: not v.location.startswith('http'), all_voices)
        available_voices = [v.key for v in local_voices]
        return yarp.SVector(available_voices)

    def say(self, text):
        self.engine.begin_utterance()
        self.engine.speak_text(text)

        for result in self.engine.end_utterance():
            self.result_queue.put(result)

        return True

    def play(self):
        return super().play()

    def pause(self):
        return super().pause()

    def stop(self):
        if self.p:
            self.p.terminate()

        return True

    def checkSayDone(self):
        return not self.is_playing

    def _process_result(self):
        while True:
            result = self.result_queue.get()

            if result is None:
                break

            wav_bytes = result.to_wav_bytes()

            if not wav_bytes:
                continue

            with tempfile.NamedTemporaryFile(mode='wb+', suffix='.wav') as wav_file:
                wav_file.write(wav_bytes)
                wav_file.seek(0)

                for play_program in reversed(PLAY_PROGRAMS):
                    play_cmd = shlex.split(play_program)

                    if not shutil.which(play_cmd[0]):
                        continue

                    play_cmd.append(wav_file.name)
                    self.is_playing = True

                    with subprocess.Popen(play_cmd) as self.p:
                        try:
                            self.p.wait()
                        except: # e.g. on keyboard interrupt
                            self.p.kill()

                    self.is_playing = False
                    break

parser = argparse.ArgumentParser(prog='speechSynthesis', description='TTS service running a Mimic 3 engine')
parser.add_argument('--voice', '-v', help='Name of voice (expected in <voices-dir>/<language>)', required=True)
parser.add_argument('--speaker', '-s', help='Name or number of speaker (default: first speaker)')
parser.add_argument('--noise-scale', type=float, help='Noise scale [0-1], default is 0.667')
parser.add_argument('--length-scale', type=float, help='Length scale (1.0 is default speed, 0.5 is 2x faster)')
parser.add_argument('--noise-w', type=float, help='Variation in cadence [0-1], default is 0.8')
parser.add_argument('--cuda', action='store_true', help='Use Onnx CUDA execution provider (requires onnxruntime-gpu)')
parser.add_argument('--port', '-p', default='/speechSynthesis', help='YARP port prefix')

args = parser.parse_args()

tts = mimic3_tts.Mimic3TextToSpeechSystem(
    mimic3_tts.Mimic3Settings(
        length_scale=args.length_scale,
        noise_scale=args.noise_scale,
        noise_w=args.noise_w,
        use_cuda=args.cuda,
    )
)

tts.voice = args.voice
tts.speaker = args.speaker

print('Preloading voice: %s' % args.voice)
tts.preload_voice(args.voice)

yarp.Network.init()

if not yarp.Network.checkNetwork():
    print('YARP network not found')
    raise SystemExit

rpc = yarp.RpcServer()
processor = TextToSpeechResponder(tts)

if not rpc.open(args.port + '/rpc:s'):
    print('Cannot open port %s' % rpc.getName())
    raise SystemExit

processor.yarp().attachAsServer(rpc)

quitRequested = False

def askToStop():
    global quitRequested
    quitRequested = True

signal.signal(signal.SIGINT, lambda signal, frame: askToStop())
signal.signal(signal.SIGTERM, lambda signal, frame: askToStop())

while not quitRequested:
    time.sleep(0.1)

rpc.interrupt()
rpc.close()

processor.result_queue.put(None)
processor.result_thread.join()
