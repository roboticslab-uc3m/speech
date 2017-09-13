#!/usr/bin/env python

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
# [on terminal 2] speechRecognition.py
# \endverbatim
#
# You can launch a 3rd terminal to read what is published via YARP port:
#
# \verbatim
# [on terminal 3] yarp read ...  /speechRecognition:o
# \endverbatim
#
# @section speechRecognition_troubleshooting Troubleshooting
#
# 1. gst-inspect-1.0 pocketsphinx
#
# 2. Check operating system sound settings
# 
# 3. Check hardware such as cables, and physical volume controls and on/off switches

from gi import pygtkcompat
import gi

import gobject

gi.require_version('Gst', '1.0')
from gi.repository import GObject, Gst
GObject.threads_init()
Gst.init(None)
    
gst = Gst
    
print("Using pygtkcompat and Gst from gi")

pygtkcompat.enable() 
pygtkcompat.enable_gtk(version='3.0')

import gtk

import yarp
import os.path



model = {}
model["english"] = "en-us"
model["spanish"] = "es"

def getRegionCode(languaje):
    regionCode = ''
    try:
        print("llega aqui")
        regionCode = self.model[languaje]
    except:
        print("Invalid Region Code.")
    return regionCode


##
# @ingroup speechRecognition
# @brief Speech Recognition callback class
class DataProcessor(yarp.PortReader):

    def setRefToFather(self,value):
        self.refToFather = value

    def read(self,connection):
        bottleIn = yarp.Bottle()
        bottleOut = yarp.Bottle()

        if not(connection.isValid()):
            print("Connection shutting down")
            return False

        if not(bottleIn.read(connection)):
            print("Error while reading from configuration input.")
            return False

        # if-then-else structure for the implemented configuration options
        if bottleIn.get(0).asString() == "setDictionary" and bottleIn.size() == 3:
            modelfile = getRegionCode(bottleIn.get(2).asString())
            lmfile = bottleIn.get(1).asString() + "-" + bottleIn.get(2).asString() + ".lm"
            dicfile = bottleIn.get(1).asString() + "-" + bottleIn.get(2).asString() + ".dic"

            ok = self.refToFather.setDictionary("dictionary/"+lmfile, "dictionary/"+dicfile, "model/"+modelfile)
            if ok:
                print("Dictionary changed to %s, %s, %s" % (lmfile, dicfile, modelfile))
                bottleOut.addString("Dictionary changed to %s, %s, %s" % (lmfile, dicfile, modelfile))
            else:
                print("Could not found dictionary. Check file names and directories.")
                bottleOut.addString("Could not found dictionary. Check file names and directories.")
        else:
            print("Invalid command received.")
            bottleOut.addString("Invalid Operation. USAGE: setDictionary {dictionaryName} {english|en|spanish|es}")

        writer = connection.getWriter()
        if writer==None:
            print("No one to reply to")
            return True
        return bottleOut.write(writer)
        return True


##
# @ingroup speechRecognition
# @brief Speech Recognition main class.
class SpeechRecognition(object):
    """Based on GStreamer/PocketSphinx Demo Application"""
    def __init__(self):
        """Initialize a SpeechRecognition object"""
        self.rf = yarp.ResourceFinder()
        self.rf.setVerbose(True)
        self.rf.setDefaultContext('speechRecognition')
        self.rf.setDefaultConfigFile('speechRecognition.ini')
        self.my_lm = self.rf.findFileByName('dictionary/follow-me-english.lm')
        self.my_dic = self.rf.findFileByName('dictionary/follow-me-english.dic')
	self.my_model = self.rf.findPath('model/en-us/')
        self.outPort = yarp.Port()
        self.configPort = yarp.RpcServer()  # Use Port() if not Python wrapper not existent!
        self.dataProcessor = DataProcessor() 
        self.dataProcessor.setRefToFather(self) # it pass reference to DataProcessor 
        self.configPort.setReader(self.dataProcessor)       
        self.outPort.open('/speechRecognition:o')
        self.configPort.open('/speechRecognition/rpc:s')
        self.init_gst()
        

    def init_gst(self):
        """Initialize the speech components"""
#        self.pipeline = gst.parse_launch('gconfaudiosrc ! audioconvert ! audioresample '
#                                        + '! vader name=vad auto-threshold=true '
#                                        + '! pocketsphinx name=asr ! fakesink')

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
        print "---"
        b = yarp.Bottle()
        msgtype = msg.get_structure().get_name()
        print msgtype # pocketsphinx 
        
        if msgtype != 'pocketsphinx':
            return

        print "hypothesis= '%s'  confidence=%s final=%s\n" % (msg.get_structure().get_value('hypothesis'), msg.get_structure().get_value('confidence'), msg.get_structure().get_value('final'))
        if msg.get_structure().get_value('final') is True:       
                text = msg.get_structure().get_value('hypothesis')        
                print text.lower()
                b.addString(text.lower())
                if text != "":
                        self.outPort.write(b)

    def setDictionary(self, lm, dic, hmm):
        print "Changing Dictionary...."
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
        print("Dictionary changed successfully (%s) (%s) (%s)"%(self.my_lm,self.my_dic,self.my_model))

        bus = self.pipeline.get_bus()
        bus.add_signal_watch()
        bus.connect('message::element', self.element_message)

        self.pipeline.set_state(gst.State.PLAYING)
        return True

yarp.Network.init()
if yarp.Network.checkNetwork() != True:
    print '[asr] error: found no yarp network (try running "yarpserver &"), bye!'
    quit()

app = SpeechRecognition()
# enter into a mainloop
loop = GObject.MainLoop()
loop.run()
