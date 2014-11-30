#!/usr/bin/env python3

import time
import copy
import json
import requests
import threading
from pylibcerebrum.serial_mux import SerialMux

CBEAM			= 'http://c-beam.cbrp3.c-base.org/rpc/'
PORT			= '/dev/serial/by-id/usb-Arduino__www.arduino.cc__0043_64938333932351806231-if00'
BAUDRATE		= 115200
STALE_THRESHOLD = 10.0
FADE_SPEED      = 0.1
DELAY           = 0.1

s = SerialMux(PORT, BAUDRATE)
print('discovering cerebrum devices')
results = []
while not results:
	results = s.discover()
print(results)
print('opening first device')
gg = s.open(0)
print('initializing device')
print(dir(gg))
print('starting event loop')

buf = [(0,0,0)]*128

def send_buf():
    while True:
        try:
            gg.ws2801.buffer = [ int(v*255) for c in buf for v in c ]
        except:
            pass
        time.sleep(DELAY)

t = threading.Thread(target=send_buf, daemon=True)
t.start()

while True:
	buf = [(255,0,0)]*128
	time.sleep(2)
	buf = [(0,255,0)]*128
	time.sleep(2)
	buf = [(0,0,255)]*128
	time.sleep(2)
	buf = [(255,255,255)]*128
	time.sleep(2)

