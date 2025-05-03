import yarp

yarp.Network.init()

if not yarp.Network.checkNetwork():
    print('[error] Please try running yarp server')
    raise SystemExit

options = yarp.Property()
options.put('device', 'speechSynthesizer_nwc_yarp')
options.put('local', '/speechSynthesizer_nwc')
options.put('remote', '/speechSynthesizer_nws')

dd = yarp.PolyDriver(options)

if not dd.isValid():
    print('[error] Device not available')
    raise SystemExit

tts = dd.viewISpeechSynthesizer()

v = yarp.DVector(1)

if tts.getPitch(v):
    print(f'Pitch: {v[0]}')

if tts.getSpeed(v):
    print(f'Speed: {v[0]}')

s = yarp.SVector(1)

if tts.getLanguage(s):
    print(f'Language: {s[0]}') # espeak: en-uk

if tts.getVoice(s):
    print(f'Voice: {s[0]}') # espeak: english-mb-en1

sound = yarp.Sound()

if tts.synthesize('Hello, world, what is your name and what is your favorite burrito?', sound):
    print(f'Got {sound.getSamples()} samples at {sound.getFrequency()} Hz '
          f'(duration: {sound.getDuration()} s, bytes per sample: {sound.getBytesPerSample()}, channels: {sound.getChannels()})')

dd.close()
