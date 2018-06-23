#!/usr/bin/python
# -*- coding: utf-8 -*-

try:
    import midi
except ImportError:
    print "Get vishnubob/python-midi from github."
    print
    raise

import time
import argparse
import santas

parser = argparse.ArgumentParser(description='Play santas.')
parser.add_argument('-t', '--transpose', type=int, default=0,
                    help='number of steps to transpose song')
parser.add_argument('-b', '--beep', action='store_true',
                    help='beep instead of ring')
parser.add_argument('filename', help='type 0 midi file to play')
args = parser.parse_args()

usecsPerQNote = 500000 # default

pattern = midi.read_midifile(args.filename)
resolution = pattern.resolution

def calculateSecsPerTick():
    secsPerTick = usecsPerQNote * 0.000001 / resolution
    return secsPerTick

secsPerTick = calculateSecsPerTick()

#inst = santas.Beep(['beep8C4AAD.local',
#                    'beep94EC03.local',
#                    'beep8C4B01.local',
#                    'beep8C4A98.local',
#                    'beep8C4AD0.local'], args.transpose)

if args.beep:
    inst = santas.Beep(['192.168.53.45',
                        '192.168.53.50',
                        '192.168.53.43',
                        '192.168.53.40',
                        '192.168.53.44'], args.transpose)
else:
    #inst = santas.Ring({'beep8C4AAD.local': (81, 83),
    #                    'beep94EC03.local': (85, 86),
    #                    'beep8C4B01.local': (88, 90),
    #                    'beep8C4A98.local': (92, 93),
    #                    'beep8C4AD0.local': (95, 97)}, args.transpose)
    inst = santas.Ring({'192.168.53.45': (81, 83),
                        '192.168.53.50': (85, 86),
                        '192.168.53.43': (88, 90),
                        '192.168.53.40': (92, 93),
                        '192.168.53.44': (95, 97)}, args.transpose)
    
for track in pattern:
    for event in track:
        time.sleep(event.tick * secsPerTick)
        if type(event) is midi.SetTempoEvent:
            usecsPerQNote = event.mpqn
            secsPerTick = calculateSecsPerTick()
        if type(event) is midi.NoteOnEvent and event.data[1] != 0:
            #print "+", event.data[0]
            inst.play(event.data[0])
        if type(event) is midi.NoteOffEvent or (type(event) is midi.NoteOnEvent and event.data[1] == 0):
            #print "-", event.data[0]
            inst.stop(event.data[0])
