#!/usr/bin/env python3

import argparse
import yarp
import roboticslab_speech as speech
import time

parser = argparse.ArgumentParser(description='create a client connection with a remote Espeak service')
parser.add_argument('--remote', default='/tts', type=str, help='remote server port')
args = parser.parse_args()

yarp.Network.init()

if not yarp.Network.checkNetwork():
    print('Please start a yarp name server first')
    raise SystemExit

client = yarp.RpcClient()

if not client.open('/tts/client/rpc:c'):
    print('Unable to open client port %s' % client.getName())
    raise SystemExit

if not yarp.Network.connect(client.getName(), args.remote + '/rpc:s'):
    print('Unable to connect to remote server port %s' % args.remote)
    raise SystemExit

tts = speech.SpeechSynthesis()
tts.yarp().attachAsClient(client)

tts.setLanguage('mb-en1')
tts.setSpeed(150) # Values 80 to 450.
tts.setPitch(60) # 50 = normal

print('Using speed %d' % tts.getSpeed())
print('Using pitch %d' % tts.getPitch())

tts.say('Hello, my name is Teo. I want to follow you. Please, tell me. Ok, I will follow you. Ok, I will stop following you.')

while not tts.checkSayDone():
    time.sleep(0.1)
