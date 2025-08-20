#!/usr/bin/env python3

##
# @ingroup speech-programs
# @defgroup chatCompletion chatCompletion.py
# @brief Starts a chat session with an LLM-GPT service.

import argparse
import time
import yarp

parser = argparse.ArgumentParser(description='YARP chat completion service', add_help=False)
parser.add_argument('--prefix', '-p', type=str, default='/chatCompletion', help='port prefix')
parser.add_argument('--context', type=str, default='chatCompletion', help='context directory')
parser.add_argument('--from', type=str, dest='ini', default='chatCompletion.ini', help='configuration (.ini) file')
parser.add_argument('--tts', type=str, default='/tts', help='TTS port prefix')
parser.add_argument('--asr', type=str, default='/asr', help='ASR port prefix')
parser.add_argument('--llm', type=str, default='/llm', help='LLM port prefix')
parser.add_argument('--player', type=str, default='/player', help='audio player port prefix')
parser.add_argument('--recorder', type=str, default='/recorder', help='audio recorder port prefix')
parser.add_argument('--wakeword', type=str, default='/wakeword', help='wake word port prefix')
args = parser.parse_args()

yarp.Network.init()

if not yarp.Network.checkNetwork():
    print('[error] Please try running yarp server')
    raise SystemExit

def connect_ports(source_port, destination_port):
    if not yarp.Network.connect(source_port, destination_port):
        print(f'Unable to connect {source_port} to {destination_port}')

def disconnect_ports(source_port, destination_port):
    if not yarp.Network.disconnect(source_port, destination_port):
        print(f'Unable to disconnect {source_port} from {destination_port}')

# TTS

tts_options = yarp.Property()
tts_options.put('device', 'speechSynthesizer_nwc_yarp')
tts_options.put('local', args.prefix + '/tts')
tts_options.put('remote', args.tts)

tts_device = yarp.PolyDriver(tts_options)

if not tts_device.isValid():
    print('[error] TTS device not available')
    raise SystemExit

tts = tts_device.viewISpeechSynthesizer()

# ASR

asr_port = yarp.BufferedPortBottle()

if not asr_port.open(args.prefix + '/asr/text:i'):
    print('[error] Unable to open ASR port')
    raise SystemExit

asr_port.setStrict(True) # don't drop messages

# wake word detection

wakeword_port = yarp.BufferedPortBottle()

if not wakeword_port.open(args.prefix + '/wakeword/result:i'):
    print('[error] Unable to open wakeword port')
    raise SystemExit

# LLM

llm_options = yarp.Property()
llm_options.put('device', 'LLM_nwc_yarp')
llm_options.put('local', args.prefix + '/llm')
llm_options.put('remote', args.llm + '/rpc:i')

llm_device = yarp.PolyDriver(llm_options)

if not llm_device.isValid():
    print('[error] LLM device not available')
    raise SystemExit

llm = llm_device.viewILLM()

# main loop

def main_loop():
    print('----- Starting main loop -----')

    sound = yarp.Sound()
    answer = yarp.LLM_Message()

    # 1. detect wake word
    print('1. Detecting wake word')
    connect_ports(args.recorder + '/audio:o', args.wakeword + '/sound:i')

    while True:
        wakeword_bottle = wakeword_port.read(False)

        if wakeword_bottle is not None:
            print(f'   -> {wakeword_bottle.toString()}')
            break

        time.sleep(0.01)

    disconnect_ports(args.recorder + '/audio:o', args.wakeword + '/sound:i')

    # 2. acknowledge readiness through TTS
    print('2. Signaling readiness through TTS')
    tts.synthesize('I am ready to listen', sound)
    time.sleep(sound.getDuration())

    # 3. wait for ASR result
    print('3. Waiting for ASR result')
    connect_ports(args.recorder + '/audio:o', args.asr + '/sound:i')

    while True:
        asr_bottle = asr_port.read(False)

        if asr_bottle is not None and asr_bottle.size() > 0 and len(asr_bottle.get(0).asString()) > 0:
            question = asr_bottle.get(0).asString()
            print(f'   -> {question}')
            break

        time.sleep(0.01)

    disconnect_ports(args.recorder + '/audio:o', args.asr + '/sound:i')

    # 4. send ASR result to LLM and wait for answer
    print('4. Sending ASR result to LLM and waiting for answer')
    llm.ask(question, answer)
    print(f'   -> {answer.content}')

    # 5. send LLM result to TTS
    print('5. Sending LLM result to TTS')
    tts.synthesize(answer.content, sound)
    time.sleep(sound.getDuration())

# prerequisites

connect_ports(args.tts + '/sound:o', args.player + '/audio:i')
connect_ports(args.asr + '/text:o', asr_port.getName())
connect_ports(args.wakeword + '/result:o', wakeword_port.getName())

disconnect_ports(args.recorder + '/audio:o', args.asr + '/sound:i')

try:
    while True:
        main_loop()
except KeyboardInterrupt:
    print('Keyboard interrupt received, stopping...')

# housekeeping

disconnect_ports(args.recorder + '/audio:o', args.wakeword + '/sound:i')
disconnect_ports(args.recorder + '/audio:o', args.asr + '/sound:i')
disconnect_ports(args.tts + '/sound:o', args.player + '/audio:i')

tts_device.close()
llm_device.close()

wakeword_port.interrupt()
wakeword_port.close()

asr_port.interrupt()
asr_port.close()

yarp.Network.fini()
