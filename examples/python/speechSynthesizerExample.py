#!/usr/bin/env python3

import argparse
import time
import yarp

parser = argparse.ArgumentParser(description='speech synthesizer example')
parser.add_argument('--remote', default='/speechSynthesizer_nws', type=str, help='remote port')

args = parser.parse_args()

yarp.Network.init()

if not yarp.Network.checkNetwork():
    print('Please start a YARP name server first')
    raise SystemExit

options = yarp.Property()
options.put('device', 'speechSynthesizer_nwc_yarp')
options.put('local', '/speechSynthesizerExample/client')
options.put('remote', args.remote)

device = yarp.PolyDriver(options)

if not device.isValid():
    print('Remote device not available')
    raise SystemExit

synthesizer = device.viewISpeechSynthesizer()

if not synthesizer:
    print('Unable to get ISpeechSynthesizer interface')
    raise SystemExit

language = yarp.SVector(1)

if not synthesizer.getLanguage(language):
    print("Failed to get current language")
    raise SystemExit

print("Current language:", language[0])

voice = yarp.SVector(1)

if not synthesizer.getVoice(voice):
    print("Failed to get current voice")
    raise SystemExit

print("Current voice:", voice[0])

speed = yarp.DVector(1)

if not synthesizer.getSpeed(speed):
    print("Failed to get current speed")
    raise SystemExit

print("Current speed:", speed[0])

pitch = yarp.DVector(1)

if not synthesizer.getPitch(pitch):
    print("Failed to get current pitch")
    # not implemented in PiperSynthesizer

print("Current pitch:", pitch[0])

if not synthesizer.setLanguage("english"):
    print("Failed to set language to English")
    raise SystemExit

print("Language set to English")

if not synthesizer.setSpeed(1.0):
    print("Failed to set speed to 1.0")
    raise SystemExit

print("Speed set to 1.0")

text = "Hello, world! This is a test of the speech synthesizer."
sound = yarp.Sound()

if not synthesizer.synthesize(text, sound):
    print("Failed to synthesize speech")
    raise SystemExit

time.sleep(4.0) # Wait for synthesis to complete

if not synthesizer.setSpeed(0.5):
    print("Failed to set speed to 0.5")
    raise SystemExit

print("Speed set to 0.5")

if not synthesizer.synthesize(text, sound):
    print("Failed to synthesize speech after setting speed to 0.5")
    raise SystemExit

time.sleep(6.0) # Wait for synthesis to complete

if not synthesizer.setSpeed(2.0):
    print("Failed to set speed to 2.0")
    raise SystemExit

print("Speed set to 2.0")

if not synthesizer.synthesize(text, sound):
    print("Failed to synthesize speech after setting speed to 2.0")
    raise SystemExit

time.sleep(2.0) # Wait for synthesis to complete

if not synthesizer.setLanguage("spanish"):
    print("Failed to set language to Spanish")
    raise SystemExit

print("Language set to Spanish")

if not synthesizer.synthesize(text, sound):
    print("Failed to synthesize speech in Spanish")
    raise SystemExit

time.sleep(4.0) # Wait for synthesis to complete

if not synthesizer.setVoice("lessac"):
    print("Failed to set voice to lessac")
    raise SystemExit

print("Voice set to lessac")

if not synthesizer.synthesize("Oh hello there!", sound):
    print("Failed to synthesize speech with lessac voice")
    raise SystemExit

time.sleep(2.0) # Wait for synthesis to complete

device.close()
