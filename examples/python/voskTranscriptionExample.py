import yarp

yarp.Network.init()

if not yarp.Network.checkNetwork():
    print('[error] Please try running yarp server')
    raise SystemExit

options = yarp.Property()
options.put('device', 'VoskTranscription')

dd = yarp.PolyDriver(options)

if not dd.isValid():
    print('[error] Device not available')
    raise SystemExit

asr = dd.viewISpeechTranscription()

if not asr.setLanguage('vosk-model-small-es-0.42'):
    print('[error] Failed to set language')
    raise SystemExit

class SoundCallback(yarp.TypedReaderCallbackSound):
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

if not p.open('/voskTranscription'):
    print('[error] Failed to open port')
    raise SystemExit

input('Press ENTER to quit\n')
print('Stopping the program...')

p.interrupt()
p.close()

dd.close()
