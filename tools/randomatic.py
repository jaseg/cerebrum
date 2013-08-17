#!/usr/bin/env python3
""" Randomly blink all led matrices on a device """
import time, random, sys
from pylibcerebrum.serial_mux import SerialMux
port, speed = sys.argv[1], 115200
if len(sys.argv) > 2:
    speed = int(sys.argv[2])

s = SerialMux(port, speed)
time.sleep(1)
print('Discovering cerebrum devices')
results = []
while not results:
    results = s.discover()
print('Opening first device')
g = s.open(0)
print('Initializing device')
print(dir(g))

led_matrices = [ m for m in g if m.type == 'led_matrix' ]
pwms         = [ m for m in g if m.type == 'simple-io' and m.config.get('mode') == 'pwm' ]

lamp_rand_data = [ [ [ random.randint(0, 1) for _ in range(28) ] for _ in led_matrices ] for _ in range(128) ]
pwm_rand_data  = [ x for _ in range(8) for x in [[ random.randint(0, 255) for _ in pwms ]]*16 ]


while True:
    for lamps, pwmvs in zip(lamp_rand_data, pwm_rand_data):
        for m, d in zip(led_matrices, lamps):
            m.buffer = d
        for m, d in zip(pwms, pwmvs):
            m.pwm = d
        time.sleep(0.2)
