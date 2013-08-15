#!/usr/bin/env python3
""" Randomly blink all led matrices on a device """
import time, random, sys
from pylibcerebrum import ganglion
port, speed = sys.argv[1], 115200
if len(sys.argv) > 2:
	speed = int(sys.argv[2])

g = ganglion.Ganglion(port, speed)
led_matrices = [m for m in g if m.type == 'led_matrix']

rand_data = [ [ [ random.randint(0, 1) for _ in range(28) ] for _ in led_matrices ] for _ in range(100) ]


while True:
	for row in rand_data:
		for m, d in zip(led_matrices, row):
			m.buffer = d
		time.sleep(0.2)

