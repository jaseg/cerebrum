#!/usr/bin/env python3

import time
import copy
import json
import requests
from pylibcerebrum.serial_mux import SerialMux

BASE_URI='http://10.0.1.43/dmxacl/json/'
PORT = '/dev/ttyUSB0'

s = SerialMux(PORT, 57600)
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

oldstate = None
cr, cg, cb = 0, 0, 0
while 1:
	if g.schnurlinks.state:
		cr = 0
	else:
		cr = 1
	if g.schnurmitte.state:
		cg = 0
	else:
		cg = 1
	if g.schnurrechts.state:
		cb = 0
	else:
		cb = 1
	if cr or cg or cb:
		res = requests.post(BASE_URI, data='{"method": "lightSync.pull", "params": [], "id": 0}').json()
		if not oldstate:
			oldstate = res['result']
		state = copy.deepcopy(res['result'])
		for v in state:
			v['red'] = 255 if cr else 0
			v['green'] = 255 if cg else 0
			v['blue'] = 255 if cb else 0
		requests.post(BASE_URI, data=json.dumps({'method': 'lightSync.push', 'params': [state], 'id': 0}))
	elif oldstate:
		requests.post(BASE_URI, data=json.dumps({'method': 'lightSync.push', 'params': [oldstate], 'id': 0}))
		oldstate = None
	time.sleep(0.1)

