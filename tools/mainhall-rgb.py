#!/usr/bin/env python3

import time
import copy
import json
import requests
import os
from pylibcerebrum.serial_mux import SerialMux

BASE_URI	= 'http://10.0.1.43/dmxacl/json/'
CBEAM		= 'http://c-beam.cbrp3.c-base.org/rpc/'
PORT		= '/dev/serial/by-id/usb-FTDI_FT232R_USB_UART_A700fmkX-if00-port0'
BAUDRATE	= 57600
GAMMA		= 2.5
CACERT		= os.path.join(os.path.dirname(__file__), 'cacert.pem')

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

def xform(o, i):
	c			= (o/255)**(1/GAMMA) #inverse gamma correction
	c			= round(c*4)
	c			= (c+i)%5
	c			= c/4
	return		  round((c**GAMMA)*255)

def inc_color(r,g,b):
	state = requests.post(BASE_URI, data='{"method": "lightSync.pull", "params": [], "id": 0}').json()['result']
	for v in state:
		v['red']	= xform(v['red'], r)
		v['green']	= xform(v['green'], g)
		v['blue']	= xform(v['blue'], b)
	try:
		requests.post(BASE_URI, data=json.dumps({'method': 'lightSync.push', 'params': [state], 'id': 0}))
	except requests.HTTPError as e:
		print('Cannot set DMX color:', e)

def sendstate(schnur, state):
	try:
		requests.post(CBEAM, data=json.dumps({'method': 'barschnur', 'params': [schnur, state], 'id': 0}), verify=CACERT)
	except requests.HTTPError as e:
		print('Cannot send event to c-beam:', e)

oldstate = None
sr, sg, sb = False, False, False
while 1:
	if g.schnurlinks.state:
		if sr:
			sendstate('links', False)
			inc_color(1,0,0)
		sr = False
	else:
		if not sr:
			sendstate('links', True)
		sr = True
	if g.schnurmitte.state:
		if sg:
			sendstate('mitte', False)
			inc_color(0,1,0)
		sg = False
	else:
		if not sg:
			sendstate('mitte', True)
		sg = True
	if g.schnurrechts.state:
		if sb:
			sendstate('rechts', False)
			inc_color(0,0,1)
		sb = False
	else:
		if not sb:
			sendstate('rechts', True)
		sb = True
	time.sleep(0.1)

