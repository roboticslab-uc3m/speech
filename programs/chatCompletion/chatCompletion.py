#!/usr/bin/env python3

import argparse
import os
import openai
import roboticslab_speech
import time
import yarp

MODEL = "gpt-3.5-turbo"

openai.api_key = os.getenv("OPENAI_API_KEY")

class SpeechRecognitionCallback(yarp.BottleCallback):
    def __init__(self):
        super().__init__()

    def onRead(self, bottle, reader):
        print("Port %s received: %s" % (reader.getName(), bottle.toString()))

parser = argparse.ArgumentParser(prog='chatCompletion', description='A simple GPT chat client')
parser.add_argument('--port', default='/chatCompletion', help='port prefix (default: /chatCompletion)')

args = parser.parse_args()

yarp.Network.init()

if not yarp.Network.checkNetwork():
    print('YARP network not found')
    raise SystemExit

asrRpcPort = yarp.RpcClient()
ttsRpcPort = yarp.RpcClient()

asrStreamPort = yarp.BufferedPortBottle()

if not asrRpcPort.open(args.port + '/asr/rpc:c'):
    print('Cannot open port %s' % asrRpcPort.getName())
    raise SystemExit

if not ttsRpcPort.open(args.port + '/tts/rpc:c'):
    print('Cannot open port %s' % ttsRpcPort.getName())
    raise SystemExit

if not asrStreamPort.open(args.port + '/asr/result:i'):
    print('Cannot open port %s' % asrStreamPort.getName())
    raise SystemExit

asr = roboticslab_speech.SpeechRecognitionIDL()
tts = roboticslab_speech.TextToSpeechIDL()
callback = SpeechRecognitionCallback()

asr.yarp().attachAsClient(asrRpcPort)
tts.yarp().attachAsClient(ttsRpcPort)
asrStreamPort.useCallback(callback)

try:
    while True:
        time.sleep(1)
except KeyboardInterrupt:
    # tts.stop()

    asrStreamPort.disableCallback()
    asrStreamPort.interrupt()
    asrStreamPort.close()

    asrRpcPort.interrupt()
    asrRpcPort.close()

    ttsRpcPort.interrupt()
    ttsRpcPort.close()