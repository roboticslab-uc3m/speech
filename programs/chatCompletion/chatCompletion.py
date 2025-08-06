#!/usr/bin/env python3

##
# @ingroup speech-programs
# @defgroup chatCompletion chatCompletion.py
# @brief Starts a chat session with an LLM-GPT service.

import argparse
import time
import yarp

VOCAB_OK = yarp.encode('ok')

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

tts_options = yarp.Property()
tts_options.put('device', 'speechSynthesizer_nwc_yarp')
tts_options.put('local', args.prefix + '/tts')
tts_options.put('remote', args.tts)

tts_device = yarp.PolyDriver(tts_options)

if not tts_device.isValid():
    print('[error] TTS device not available')
    raise SystemExit

tts = tts_device.viewISpeechSynthesizer()

llm_options = yarp.Property()
llm_options.put('device', 'LLM_nwc_yarp')
llm_options.put('local', args.prefix + '/llm')
llm_options.put('remote', args.llm + '/rpc:i')

llm_device = yarp.PolyDriver(llm_options)

if not llm_device.isValid():
    print('[error] LLM device not available')
    raise SystemExit

llm = llm_device.viewILLM()

player_rpc = yarp.RpcClient()

if not player_rpc.open(args.prefix + '/player/rpc:o'):
    print('[error] Unable to open player RPC port')
    raise SystemExit

if not yarp.Network.connect(player_rpc.getName(), args.player + '/rpc:i'):
    print('[error] Unable to connect player RPC port')
    raise SystemExit

recorder_rpc = yarp.RpcClient()

if not recorder_rpc.open(args.prefix + '/recorder/rpc:o'):
    print('[error] Unable to open recorder RPC port')
    raise SystemExit

if not yarp.Network.connect(recorder_rpc.getName(), args.recorder + '/rpc'):
    print('[error] Unable to connect recorder RPC port')
    raise SystemExit

wakeword_port = yarp.BufferedPortBottle()

if not wakeword_port.open(args.prefix + '/wakeword/result:i'):
    print('[error] Unable to open wakeword port')
    raise SystemExit

if not yarp.Network.connect(args.wakeword + '/result:o', wakeword_port.getName()):
    print('[error] Unable to connect wakeword port')
    raise SystemExit

asr_port = yarp.BufferedPortBottle()

if not asr_port.open(args.prefix + '/asr/text:i'):
    print('[error] Unable to open ASR port')
    raise SystemExit

if not yarp.Network.connect(args.asr + '/text:o', asr_port.getName()):
    print('[error] Unable to connect ASR port')
    raise SystemExit

def set_player_state(*, on):
    response = yarp.Bottle()
    player_rpc.write(yarp.Bottle('start' if on == True else 'stop'), response)
    if response.size() == 0 or response.get(0).asString() != 'ok':
        print(f'[error] Failed to set player state to {"on" if on else "off"}')

def set_recorder_state(*, on):
    response = yarp.Bottle()
    recorder_rpc.write(yarp.Bottle('startRecording_RPC' if on == True else 'stopRecording_RPC'), response)
    if response.size() != 0:
        print(f'[error] Failed to set recorder state to {"on" if on else "off"}')
    else: reset_recorder_buffer()

def reset_recorder_buffer():
    response = yarp.Bottle()
    recorder_rpc.write(yarp.Bottle('resetRecordingAudioBuffer_RPC'), response)
    if response.size() != 0:
        print('[error] Failed to reset recorder audio buffer')

def main_loop():
    sound = yarp.Sound()

    # 1. disable speaker, enable microphone
    print('1. Disabling speaker, enabling microphone')
    set_player_state(on=False)
    set_recorder_state(on=True)

    # 2. wait for wakeword
    print('2. Waiting for wakeword')

    while True:
        wakeword_bottle = wakeword_port.read(False)

        if wakeword_bottle is not None:
            print(f'Wakeword detected: {wakeword_bottle.toString()}')
            break

        time.sleep(0.1)

    # 3. disable microphone, enable speaker
    print('3. Disabling microphone, enabling speaker')
    set_recorder_state(on=False)
    set_player_state(on=True)

    # 4. acknowledge readiness through TTS
    print('4. Signaling readiness through TTS')
    tts.synthesize('I am ready to listen', sound)
    time.sleep(sound.getDuration())

    # 5. disable speaker, enable microphone
    print('5. Disabling speaker, enabling microphone')
    set_player_state(on=False)
    set_recorder_state(on=True)

    # 6. wait for ASR
    print('6. Waiting for ASR')

    while True:
        asr_bottle = asr_port.read(False)

        if asr_bottle is not None and asr_bottle.size() > 0 and len(asr_bottle.get(0).asString()) > 0:
            question = asr_bottle.get(0).asString()
            print(f'ASR result: {question}')
            break

        time.sleep(0.1)

    # 7. disable speaker, disable microphone
    print('7. Disabling speaker, disabling microphone')
    set_player_state(on=False)
    set_recorder_state(on=False)

    # 8. send ASR result to LLM
    print('8. Sending ASR result to LLM')
    answer = yarp.SVector(1) # FIXME: should be a LLM_Message, but its bindings are missing
    llm.ask(question, answer)

    # 9. enable speaker, disable microphone
    print('9. Enabling speaker, disabling microphone')
    set_player_state(on=True)
    set_recorder_state(on=False)

    # 10. send LLM result to TTS
    print('10. Sending LLM result to TTS')
    tts.synthesize(answer[0], sound)
    time.sleep(sound.getDuration())

try:
    while True: main_loop()
except KeyboardInterrupt:
    print('Keyboard interrupt received, stopping...')

tts_device.close()
llm_device.close()

player_rpc.interrupt()
player_rpc.close()

recorder_rpc.interrupt()
recorder_rpc.close()

wakeword_port.interrupt()
wakeword_port.close()

asr_port.interrupt()
asr_port.close()

yarp.Network.fini()
