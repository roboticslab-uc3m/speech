#!/usr/bin/env python3

## @ingroup speech-examples-py
#  @defgroup voskTranscriptionExamplePy voskTranscriptionExample.py
#  @see @ref voskTranscriptionExample.py
## @example{lineno} voskTranscriptionExample.py

import argparse
import yarp

parser = argparse.ArgumentParser(description='speech transcription example')
parser.add_argument('--model', default='vosk-model-small-es-0.42', type=str, help='language model directory')
parser.add_argument('--useGPU', action='store_true', help='use GPU if available')
parser.add_argument('--port', default='/voskTranscriptionExample/sound:i', type=str, help='port name for sound input')

args = parser.parse_args()

yarp.Network.init()

if not yarp.Network.checkNetwork():
    print('[error] Please try running yarp server')
    raise SystemExit

options = yarp.Property()
options.put('device', 'VoskTranscription')
options.put('model', args.model)

if args.useGPU:
    options.put('useGPU', True)

dd = yarp.PolyDriver(options)

if not dd.isValid():
    print('[error] Device not available')
    raise SystemExit

asr = dd.viewISpeechTranscription()

if not asr.setLanguage('vosk-model-small-es-0.42'):
    print('[error] Failed to set language')
    raise SystemExit

class SoundCallback(yarp.SoundCallback):
    def __init__(self):
        super().__init__()
        self.s = yarp.SVector(1)
        self.d = yarp.DVector(1)

    def onRead(self, sound, reader):
        if asr.transcribe(sound, self.s, self.d) and self.s[0]:
            print('Transcription:', self.s[0])

p = yarp.BufferedPortSound()
c = SoundCallback()
p.useCallback(c)

if not p.open(args.port):
    print('[error] Failed to open port')
    raise SystemExit

input('Press ENTER to quit\n')
print('Stopping the program...')

p.interrupt()
p.close()

dd.close()
