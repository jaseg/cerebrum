#!/usr/bin/env python3
import time, glob
from pylibcerebrum import *

#ADC reference voltage
FULLSCALE = 1.1

g = pylibcerebrum.ganglion(next(glob.iglob('/dev/serial/by-id/*'))
while True:
	x = g.analog0.analog/255.0 * FULLSCALE
	y = g.analog1.analog/255.0 * FULLSCALE
	print(x, '\t', y)
