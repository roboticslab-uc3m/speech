#!/usr/bin/env python3

##
# @ingroup speech-programs
# \defgroup speechRecognition speechRecognition.py
# @brief Provides basic ASR capabilities.
#
# @section speechRecognition_legal Legal
#
# Copyright: 2016-present (c) edits by Santiago Morante, Juan G Victores and Raul de Santos; past-2008 (c) Carnegie Mellon University.
#
# CopyPolicy: You may modify and redistribute this file under the same terms as the CMU Sphinx system. See
# http://cmusphinx.sourceforge.net/html/LICENSE for more information.
#
# @section speechRecognition_running Running (assuming correct installation)
#
# First we must run a YARP name server if it is not running in our current namespace:
#
# \verbatim
# [on terminal 1] yarp server
# \endverbatim
#
# Now launch the program:
#
# \verbatim
# [on terminal 2] speechRecognition
# \endverbatim
#
# You can launch a 3rd terminal to read what is published via YARP port:
#
# \verbatim
# [on terminal 3] yarp read ...  /speechRecognition:o
# \endverbatim
#
# or you can send commands to configure the speech recognition app. To know how to do that, use "help":
# \verbatim
# [on terminal 4] yarp rpc /speechRecognition/rpc:s
# [on terminal 4] >>help
# \endverbatim
#
# For instance, you can load the 20k words Spanish dictionary with:
# \verbatim
# [on terminal 4] >>setDictionary 20k es
# \endverbatim
#
# @section speechRecognition_troubleshooting Troubleshooting
#
# 1. gst-inspect-1.0 pocketsphinx
#
# 2. Check operating system sound settings
#
# 3. Check hardware such as cables, and physical volume controls and on/off switches

import gi

gi.require_version('Gst', '1.0')
from gi.repository import GObject, Gst
GObject.threads_init()
Gst.init(None)

gst = Gst

from gi import pygtkcompat
print('Using pygtkcompat and Gst from gi')

pygtkcompat.enable()
pygtkcompat.enable_gtk(version='3.0')

import yarp
import os.path

# For alsaaudio
import sys
import alsaaudio

import roboticslab_speech


##
# @ingroup speechRecognition
# @brief Speech Recognition callback class
class SpeechRecognitionResponder(roboticslab_speech.SpeechRecognitionIDL):
    def __init__(self, owner):
        super().__init__()
        self.owner = owner
        self.mixer = None

    def setDictionary(self, dictionary, language):
        print('Setting dictionary to %s (language: %s)' % (dictionary, language))

        lm_path = 'dictionary/%s-%s.lm' % (dictionary, language)
        dic_path = 'dictionary/%s-%s.dic' % (dictionary, language)
        model_path = 'model/' + language

        if not self.owner.setDictionary(lm_path, dic_path, model_path):
            print('Unable to set dictionary')
            return False

        return True

    def muteMicrophone(self):
        print('Muting microphone')

        if not self._initMixer():
            return False

        self.mixer.setrec(0, alsaaudio.MIXER_CHANNEL_ALL)
        return True

    def unmuteMicrophone(self):
        print('Unmuting microphone')

        if not self._initMixer():
            return False

        self.mixer.setrec(1, alsaaudio.MIXER_CHANNEL_ALL)
        return True

    def _initMixer(self):
        if self.mixer is None:
            try:
                self.mixer = alsaaudio.Mixer('Capture') # Capture / Master
            except alsaaudio.ALSAAudioError:
                print('No such mixer: %s' % sys.stderr)
                return False

        return True


##
# @ingroup speechRecognition
# @brief Speech Recognition main class.
class SpeechRecognition(object):
    """Based on GStreamer/PocketSphinx Demo Application"""

    def __init__(self, rf):
        """Initialize a SpeechRecognition object"""
        self.rf = rf
        self.my_lm = self.rf.findFileByName('dictionary/follow-me-en-us.lm')
        self.my_dic = self.rf.findFileByName('dictionary/follow-me-en-us.dic')
        self.my_model = self.rf.findPath('model/en-us/')
        self.outPort = yarp.BufferedPortBottle()
        self.configPort = yarp.RpcServer()
        self.dataProcessor = SpeechRecognitionResponder(self)
        self.outPort.open('/speechRecognition:o')
        self.configPort.open('/speechRecognition/rpc:s')
        self.dataProcessor.yarp().attachAsServer(self.configPort)
        self.init_gst()

    def init_gst(self):
        """Initialize the speech components"""
        #self.pipeline = gst.parse_launch('gconfaudiosrc ! audioconvert ! audioresample '
        #                                  + '! vader name=vad auto-threshold=true '
        #                                  + '! pocketsphinx name=asr ! fakesink')

        """ Configuring the decoder and improving accuracy """
        self.pipeline = gst.parse_launch('autoaudiosrc ! audioconvert ! audioresample '
                                        + '! pocketsphinx name=asr beam=1e-20 ! fakesink')
        asr = self.pipeline.get_by_name('asr')
        # asr.connect('result', self.asr_result) (it's not running with Gstreamer 1.0)
        asr.set_property('lm', self.my_lm )
        asr.set_property('dict', self.my_dic )
        asr.set_property('hmm', self.my_model )
        #asr.set_property('configured', "true")

        bus = self.pipeline.get_bus()
        bus.add_signal_watch()
        bus.connect('message::element', self.element_message) # new

        self.pipeline.set_state(gst.State.PLAYING)

    def element_message(self, bus, msg):
        """Receive element messages from the bus."""
        print('---')
        msgtype = msg.get_structure().get_name()
        print(msgtype) # pocketsphinx

        if msgtype != 'pocketsphinx':
            return

        print("hypothesis= '%s' confidence=%s final=%s\n" % (
            msg.get_structure().get_value('hypothesis'),
            msg.get_structure().get_value('confidence'),
            msg.get_structure().get_value('final')
        ))

        if msg.get_structure().get_value('final') is True:
            text = msg.get_structure().get_value('hypothesis')

            if text != '':
                print(text.lower())
                b = self.outPort.prepare()
                b.clear()
                b.addString(text.lower())
                self.outPort.write()

    def setDictionary(self, lm, dic, hmm):
        lm = self.rf.findFileByName(lm)
        dic = self.rf.findFileByName(dic)
        model = self.rf.findFileByName(hmm)

        if lm == '' or dic == '' or model == '':
            return False

        self.my_lm = lm
        self.my_dic = dic
        self.my_model = model

        self.pipeline.set_state(gst.State.NULL)
        self.pipeline = gst.parse_launch('autoaudiosrc ! audioconvert ! audioresample '
                                         + '! pocketsphinx name=asr beam=1e-20 ! fakesink')

        asr = self.pipeline.get_by_name('asr')
        asr.set_property('lm', self.my_lm)
        asr.set_property('dict', self.my_dic)
        asr.set_property('hmm', self.my_model )

        print('-------------------------------')
        print('Dictionary changed successfully (%s) (%s) (%s)' % (self.my_lm, self.my_dic, self.my_model))

        bus = self.pipeline.get_bus()
        bus.add_signal_watch()
        bus.connect('message::element', self.element_message)

        self.pipeline.set_state(gst.State.PLAYING)
        return True


rf = yarp.ResourceFinder()
rf.setDefaultContext('speechRecognition')
rf.setDefaultConfigFile('speechRecognition.ini')

yarp.Network.init()

if not yarp.Network.checkNetwork():
    print('[asr] error: found no yarp network (try running "yarpserver &")')
    raise SystemExit

app = SpeechRecognition(rf)

# enter into a mainloop
loop = GObject.MainLoop()
loop.run()
