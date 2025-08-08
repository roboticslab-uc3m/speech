#!/usr/bin/env python3

##
# @ingroup speech-programs
# @defgroup wakeWord wakeWord.py
# @brief Launched a wake word detector.

from openwakeword.model import Model

import argparse
import numpy as np
import os
import signal
import time
import yarp

parser = argparse.ArgumentParser(description='YARP wake word service', add_help=False)
parser.add_argument('--model', type=str, required=True, help='path to .onnx model file')
parser.add_argument('--confidence', type=float, default=0.8, help='detection confidence threshold (0-1)')
parser.add_argument('--prefix', '-p', type=str, default='/wakeWord', help='YARP port prefix')
parser.add_argument('--context', type=str, default='wakeWord', help='YARP context directory')
parser.add_argument('--from', type=str, dest='ini', default='wakeWord.ini', help='YARP configuration (.ini) file')
args = parser.parse_args()

yarp.Network.init()

rf = yarp.ResourceFinder()
rf.setDefaultContext(args.context)
rf.setDefaultConfigFile(args.ini)

if not yarp.Network.checkNetwork():
    print('[error] Please try running yarp server')
    raise SystemExit

onnx_model_path = rf.findFileByName(args.model)

if not onnx_model_path:
    print(f'[error] Failed to find model file {args.model}')
    raise SystemExit

print(f'Using model {onnx_model_path}')
model = Model(wakeword_model_paths=[onnx_model_path])

pb = yarp.BufferedPortBottle()

if not pb.open(args.prefix + '/result:o'):
    print('[error] Failed to open bottle out port')
    raise SystemExit

class SoundCallback(yarp.SoundCallback):
    def __init__(self, model, model_name, threshold):
        super().__init__()
        self.model = model
        self.model_name = os.path.basename(model_name[:-len('.onnx')])
        self.threshold = threshold

    def onRead(self, sound, reader):
        if sound.getFrequency() == 16000 and sound.getBytesPerSample() == 2:
            frame = np.empty(sound.getSamples(), dtype=np.int16)

            for i in range(sound.getSamples()):
                frame[i] = sound.get(i)

            confidence = self.model.predict(frame)[self.model_name]
            print(f'{self.model_name}: {confidence}')

            if confidence >= self.threshold and pb.getOutputCount() > 0:
                bottle = pb.prepare()
                bottle.clear()
                bottle.addString(self.model_name)
                bottle.addFloat32(float(confidence))

                pb.write()

ps = yarp.BufferedPortSound()
c = SoundCallback(model, args.model, args.confidence)
ps.useCallback(c)

if not ps.open(args.prefix + '/sound:i'):
    print('[error] Failed to open sound in port')
    raise SystemExit

should_stop = False

def signal_handler(signum, frame):
    global should_stop
    should_stop = True

signal.signal(signal.SIGINT, signal_handler)

while not should_stop:
    if ps.getInputCount() == 0 and model.prediction_buffer:
        model.reset() # flush prediction buffer

    time.sleep(0.1)

print('Stopping the program...')

ps.interrupt()
ps.close()

pb.interrupt()
pb.close()

yarp.Network.fini()
