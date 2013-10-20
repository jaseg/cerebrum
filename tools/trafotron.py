#!/usr/bin/env python3

import time
import copy
import json
import requests
from pylibcerebrum.serial_mux import SerialMux

CBEAM			= 'http://10.0.1.27:4254/rpc/'
HYPERBLAST		= 'http://10.0.1.23:1337/'
PORT			= '/dev/serial/by-id/usb-Arduino__www.arduino.cc__0043_7523230313535161C072-if00'
BAUDRATE		= 115200
AVG_SAMPLES		= 128
SEND_THRESHOLD	= 3

s = SerialMux(PORT, BAUDRATE)
print('discovering cerebrum devices')
results = []
while not results:
	results = s.discover()
print(results)
print('opening first device')
g = s.open(0)
print('initializing device')
print(dir(g))
print('starting event loop')

def sendstate(value):
	print('SENDING', value)
	requests.post(CBEAM, data=json.dumps({'method': 'trafotron', 'params': [value], 'id': 0}))

# Set Arduino digital pin 2 to output
g.digital2.state = 1
oldval = -2*SEND_THRESHOLD
oldbarstate = None
newbarstate = None
while True:
	val = sum([ g.analog0.analog for i in range(AVG_SAMPLES)])/AVG_SAMPLES
	if abs(val-oldval) > SEND_THRESHOLD:
		oldval = val
		sendstate(int(val))
	if g.digital2.state:
		newbarstate = 'closed'
	else:
		newbarstate = 'open'
	if newbarstate != oldbarstate:
		requests.post(CBEAM, data=json.dumps({'method': 'barstatus', 'params': [newbarstate], 'id': 0}))
		requests.post(HYPERBLAST, data=json.dumps({'method': 'barstatus', 'params': [newbarstate], 'id': 0}))
		oldbarstate = newbarstate

