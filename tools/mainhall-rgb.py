#!/usr/bin/env python3

import time
import copy
import json
import requests
from pylibcerebrum.serial_mux import SerialMux

BASE_URI	='http://10.0.1.43/dmxacl/json/'
PORT		= '/dev/ttyUSB0'
BAUDRATE	= 57600
GAMMA		= 2.5

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
g.schnurlinks.state = 1
g.schnurmitte.state = 1
g.schnurrechts.state = 1
print('starting event loop')

def inc_color(r,g,b):
	state = requests.post(BASE_URI, data='{"method": "lightSync.pull", "params": [], "id": 0}').json()['result']
	for v in state:
		cr = ((round(v['red']	/64.0) + r) % 5)/4
		cg = ((round(v['green']	/64.0) + g) % 5)/4
		cb = ((round(v['blue']	/64.0) + b) % 5)/4
		cr = cr ** GAMMA
		cg = cg ** GAMMA
		cb = cb ** GAMMA
		v['red']	= round(cr*255)
		v['green']	= round(cg*255)
		v['blue']	= round(cb*255)
	requests.post(BASE_URI, data=json.dumps({'method': 'lightSync.push', 'params': [state], 'id': 0}))

oldstate = None
sr, sg, sb = False, False, False
while 1:
	if g.schnurlinks.state:
		if sr:
			inc_color(1,0,0)
		sr = False
	else:
		sr = True
	if g.schnurmitte.state:
		if sg:
			inc_color(0,1,0)
		sg = False
	else:
		sg = True
	if g.schnurrechts.state:
		if sb:
			inc_color(0,0,1)
		sb = False
	else:
		sb = True
	time.sleep(0.1)

