#!/usr/bin/env python3

import time
import copy
import json
import requests
import threading
from pylibcerebrum.serial_mux import SerialMux

CBEAM			= 'http://10.0.1.27:4254/rpc/'
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

buf = [(0,0,0)]*48
timestamps = [0]*len(buf)

def hsv2rgb(h, s, v):
    if s == 0.0:
        return v, v, v
    i = int(h*6.0) # XXX assume int() truncates!
    f = (h*6.0) - i
    p = v*(1.0 - s)
    q = v*(1.0 - s*f)
    t = v*(1.0 - s*(1.0-f))
    i = i%6
    if i == 0:
        return v, t, p
    if i == 1:
        return q, v, p
    if i == 2:
        return p, v, t
    if i == 3:
        return p, q, v
    if i == 4:
        return t, p, v
    if i == 5:
        return v, p, q

def send_buf():
    while True:
        ct = time.time()
        for i in range(len(buf)):
            d = ct-timestamps[i]
            if d > STALE_THRESHOLD:
                #buf[i] = hsv2rgb((FADE_SPEED*d + i/len(buf))%1, 1, 1)
                buf[i] = hsv2rgb((FADE_SPEED*d)%1, 1, 1)
        try:
            gg.ws2801.buffer = [ int(v*255) for c in buf for v in c ]
        except:
            pass
        time.sleep(DELAY)

t = threading.Thread(target=send_buf)
t.start()

def set_relay(relay, r, g, b):
    buf[relay] = (r,g,b)
    timestamps[relay] = time.time()

