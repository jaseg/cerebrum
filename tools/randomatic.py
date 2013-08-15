#!/usr/bin/env python3
""" Randomly blink all led matrices on a device """
import time, random, sys
from pylibcerebrum import ganglion
port, speed = sys.argv[1], 115200
if len(sys.argv) > 2:
    speed = int(sys.argv[2])

g = ganglion.Ganglion(port, speed)

c = 0
while True:
    for m in g:
        if m.type == 'led_matrix':
            m.buffer = [ random.randint(0, 1) for i in range(28)]
        elif m.type == 'simple-io' and m.config['mode'] == 'pwm' and c == 0:
            m.pwm = random.randint(0, 255)
    time.sleep(0.2)
    c += 1
    if c == 15:
        c = 0
