#!/usr/bin/env python3

# borrowed from https://github.com/alphacep/vosk-api/blob/12f29a3415e4967e088ed09202bfb0007e5a1787/python/example/test_microphone.py

import argparse
import json
import queue
import roboticslab_speech
import sounddevice as sd
import yarp

from vosk import Model, KaldiRecognizer

class VoskSpeechRecognitionResponder(roboticslab_speech.SpeechRecognitionIDL):
    def __init__(self, device, model, samplerate, stream):
        super().__init__()

        if model is None:
            self.model = Model(lang='en-us')
        else:
            self.model = Model(model_name='vosk-model-' + model)

        if samplerate is None:
            device_info = sd.query_devices(device, 'input')
            # soundfile expects an int, sounddevice provides a float:
            self.samplerate = int(device_info['default_samplerate'])
        else:
            self.samplerate = samplerate

        self.rec = KaldiRecognizer(self.model, self.samplerate)
        self.stream = stream

    def setDictionary(self, dictionary, language):
        try:
            if dictionary is not None and str(dictionary):
                print('Setting dictionary to %s' % dictionary)
                self.model = Model(model_name='vosk-model-' + dictionary)
            elif language is not None and str(language):
                print('Setting language to %s' % language)
                self.model = Model(lang=language)
            else:
                print('No dictionary or language specified')
                return False
        except SystemExit:
            print('Dictionary or language not available')
            return False

        self.rec = KaldiRecognizer(self.model, self.samplerate)
        return True

    def muteMicrophone(self):
        print('Muting microphone')
        self.stream.abort()
        return True

    def unmuteMicrophone(self):
        print('Unmuting microphone')
        self.stream.start()
        return True

    def transcribe(self, data):
        if self.rec.AcceptWaveform(data):
            return (False, json.loads(self.rec.Result())['text'])
        else:
            return (True, json.loads(self.rec.PartialResult())['partial'])

def int_or_str(text):
    """Helper function for argument parsing."""
    try:
        return int(text)
    except ValueError:
        return text

parser = argparse.ArgumentParser(description='ASR service using Vosk API and YARP', add_help=False)
parser.add_argument('--list-devices', '-l', action='store_true', help='list available audio devices and exit')
args, remaining = parser.parse_known_args()

if args.list_devices:
    print(sd.query_devices())
    raise SystemExit

parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter, parents=[parser])
parser.add_argument('--device', '-d', type=int_or_str, help='input device (numeric ID or substring)')
parser.add_argument('--samplerate', '-r', type=int, help='sampling rate')
parser.add_argument('--model', '-m', type=str, help='language model, e.g. small-es-0.42')
args = parser.parse_args(remaining)

yarp.Network.init()

if not yarp.Network.checkNetwork():
    print('no YARP network available')
    raise SystemExit

asrPort = yarp.BufferedPortBottle()
configPort = yarp.RpcServer()

if not asrPort.open('/speechRecognition:o'):
    print('Unable to open output port')
    raise SystemExit

if not configPort.open('/speechRecognition/rpc:s'):
    print('Unable to open RPC port')
    raise SystemExit

try:
    q = queue.Queue()

    with sd.RawInputStream(samplerate=args.samplerate,
                           blocksize = 8000,
                           device=args.device,
                           dtype='int16',
                           channels=1,
                           callback=lambda indata, frames, time, status: q.put(bytes(indata))) as stream:
        dataProcessor = VoskSpeechRecognitionResponder(args.device, args.model, args.samplerate, stream)
        dataProcessor.yarp().attachAsServer(configPort)

        while True:
            isPartial, transcription = dataProcessor.transcribe(q.get())

            if transcription:
                if not isPartial:
                    print('result: %s' % transcription)
                    b = asrPort.prepare()
                    b.clear()
                    b.addString(transcription)
                    asrPort.write()
                else:
                    print('partial: %s' % transcription)
except KeyboardInterrupt:
    asrPort.interrupt()
    asrPort.close()
    configPort.interrupt()
    configPort.close()
    parser.exit(0)
