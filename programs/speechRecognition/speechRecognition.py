#!/usr/bin/env python3

# ##
# # @ingroup speech-programs
# # \defgroup speechRecognition speechRecognition.py
# # @brief Provides basic ASR capabilities.
# #
# # @section speechRecognition_legal Legal
# #
# # Copyright: 2016-present (c) edits by Santiago Morante, Juan G Victores and Raul de Santos; past-2008 (c) Carnegie Mellon University.
# #
# # CopyPolicy: You may modify and redistribute this file under the same terms as the CMU Sphinx system. See
# # http://cmusphinx.sourceforge.net/html/LICENSE for more information.
# #
# # @section speechRecognition_running Running (assuming correct installation)
# #
# # First we must run a YARP name server if it is not running in our current namespace:
# #
# # \verbatim
# # [on terminal 1] yarp server
# # \endverbatim
# #
# # Now launch the program:
# #
# # \verbatim
# # [on terminal 2] speechRecognition
# # \endverbatim
# #
# # You can launch a 3rd terminal to read what is published via YARP port:
# #
# # \verbatim
# # [on terminal 3] yarp read ...  /speechRecognition:o
# # \endverbatim
# #
# # or you can send commands to configure the speech recognition app. To know how to do that, use "help":
# # \verbatim
# # [on terminal 4] yarp rpc /speechRecognition/rpc:s
# # [on terminal 4] >>help
# # \endverbatim
# #
# # For instance, you can load the 20k words Spanish dictionary with:
# # \verbatim
# # [on terminal 4] >>setDictionary 20k es
# # \endverbatim
# #
# # @section speechRecognition_troubleshooting Troubleshooting
# #
# # 1. gst-inspect-1.0 pocketsphinx
# #
# # 2. Check operating system sound settings
# #
# # 3. Check hardware such as cables, and physical volume controls and on/off switches

# borrowed from https://github.com/alphacep/vosk-api/blob/12f29a3/python/example/test_microphone.py
# and https://github.com/cmusphinx/pocketsphinx/blob/b16c631/examples/live.py

import argparse
import queue
import roboticslab_speech
import sounddevice as sd
import yarp

from pocketsphinx import Endpointer, Decoder

class PocketSphinxSpeechRecognitionResponder(roboticslab_speech.SpeechRecognitionIDL):
    def __init__(self, device, dictionary, language, stream):
        super().__init__()

        self.rf = yarp.ResourceFinder()
        self.rf.setDefaultContext('speechRecognition')
        self.rf.setDefaultConfigFile('speechRecognition.ini')

        print('Loading dictionary %s for language %s' % (dictionary, language))

        self.lm = self.rf.findFileByName('dictionary/%s-%s.lm' % (dictionary, language))
        self.dic = self.rf.findFileByName('dictionary/%s-%s.dic' % (dictionary, language))
        self.model = self.rf.findPath('model/%s' % language)

        device_info = sd.query_devices(device, 'input')
        # soundfile expects an int, sounddevice provides a float:
        self.samplerate = int(device_info['default_samplerate'])

        self.ep = Endpointer(sample_rate=self.samplerate)

        self.decoder = Decoder(
            hmm=self.model,
            lm=self.lm,
            dict=self.dic,
            samprate=self.ep.sample_rate,
            #loglevel='INFO' # might be a bit too noisy
        )

        self.stream = stream

    def setDictionary(self, dictionary, language):
        self.stream.abort()
        print('Setting dictionary to %s (language: %s)' % (dictionary, language))

        try:
            lm = self.rf.findFileByName('dictionary/%s-%s.lm' % (dictionary, language))
            dic = self.rf.findFileByName('dictionary/%s-%s.dic' % (dictionary, language))
            model = self.rf.findFileByName('model/%s' % language)

            if not lm or not dic or not model:
                print('Unable to find dictionary')
                return False

            self.lm = lm
            self.dic = dic
            self.model = model

            self.decoder = Decoder(
                hmm=self.model,
                lm=self.lm,
                dict=self.dic,
                samprate=self.ep.sample_rate
            )

            return True
        finally:
            self.stream.start()

    def muteMicrophone(self):
        print('Muting microphone')
        self.stream.abort()
        return True

    def unmuteMicrophone(self):
        print('Unmuting microphone')
        self.stream.start()
        return True

    def transcribe(self, frame):
        prev_in_speech = self.ep.in_speech
        speech = self.ep.process(frame)

        if speech is not None:
            if not prev_in_speech:
                self.decoder.start_utt()

            self.decoder.process_raw(speech)
            hyp = self.decoder.hyp()

            if not self.ep.in_speech:
                self.decoder.end_utt()
                return False, self.decoder.hyp().hypstr
            elif hyp is not None:
                # partial result
                return True, hyp.hypstr
        return False, None

def int_or_str(text):
    """Helper function for argument parsing."""
    try:
        return int(text)
    except ValueError:
        return text

parser = argparse.ArgumentParser(description='ASR service using PocketSphinx and YARP', add_help=False)
parser.add_argument('--list-devices', '-l', action='store_true', help='list available audio devices and exit')
args, remaining = parser.parse_known_args()

if args.list_devices:
    print(sd.query_devices())
    raise SystemExit

parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter, parents=[parser])
parser.add_argument('--device', '-d', type=int_or_str, help='input device (numeric ID or substring)')
parser.add_argument('--dictionary', type=str, required=True, help='dictionary, e.g. follow-me')
parser.add_argument('--language', type=str, required=True, help='language, e.g. en-us')
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

    with sd.RawInputStream(blocksize=int(2880 / 2), # FIXME: hardcoded
                           device=args.device,
                           dtype='int16',
                           channels=1,
                           callback=lambda indata, frames, time, status: q.put(bytes(indata))) as stream:
        dataProcessor = PocketSphinxSpeechRecognitionResponder(args.device, args.dictionary, args.language, stream)
        dataProcessor.yarp().attachAsServer(configPort)

        while True:
            frame = q.get()
            isPartial, transcription = dataProcessor.transcribe(frame)

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
