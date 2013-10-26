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
#bar status outputs
for io in [g.digital3, g.digital5, g.digital6, g.digital9, g.digital10, g.digital11]:
	io.direction = 1
	io.pwm_enabled = 1
print('starting event loop')

#HACK ctrl-c ctrl-p -ed from barstatus.py
barstatus = 'closed'
lastchange = time.time()
def animate():
	global barstatus, lastchange
	while True:
		lookup = barstatus
		if time.time() - lastchange < 180:
			if barstatus == 'open':
				lookup = 'opened'
			elif barstatus == 'closed':
				lookup = 'lastcall'
		l1, r1 = {'open': ((10, 255, 10), (128, 128, 128)),
			'opened': ((10, 128, 255), (128, 128, 128)),
			'closed': ((128, 128, 128), (255, 4, 4)),
		      'lastcall': ((10, 255, 10), (128, 128, 128))}.get(lookup)
		l2, r2 = {'open': ((10, 255, 10), (128, 128, 128)),
			'opened': ((10, 255, 10), (128, 128, 128)),
			'closed': ((128, 128, 128), (255, 4, 4)),
		      'lastcall': ((10, 255, 10), (255, 255, 10))}.get(lookup)
		(g.digital3.pwm, g.digital5.pwm, g.digital6.pwm), (g.digital9.pwm, g.digital10.pwm, g.digital11.pwm) = l1, r1
		time.sleep(0.33)
		(g.digital3.pwm, g.digital5.pwm, g.digital6.pwm), (g.digital9.pwm, g.digital10.pwm, g.digital11.pwm) = l2, r2
		time.sleep(0.66)

animator = Thread(target=animate)
animator.daemon = False
animator.start()


def sendstate(value):
	print('SENDING', value)
	requests.post(CBEAM, data=json.dumps({'method': 'trafotron', 'params': [value], 'id': 0}))

# Enable pull-up on Arduino analog pin 1
g.analog1.state = 1
oldval = -2*SEND_THRESHOLD
oldbarstate = None
newbarstate = None
while True:
	val = sum([ g.analog0.analog for i in range(AVG_SAMPLES)])/AVG_SAMPLES
	if abs(val-oldval) > SEND_THRESHOLD:
		oldval = val
		sendstate(int(val))
	if g.analog1.state:
		newbarstate = 'closed'
	else:
		newbarstate = 'open'
	if newbarstate != oldbarstate:
		oldbarstate = newbarstate

		#comm with animation thread
		global barstatus
		barstatus = status
		global lastchange
		lastchange = time.time()

		requests.post(HYPERBLAST, data=json.dumps({'method': 'barstatus', 'params': [newbarstate], 'id': 0}))
		requests.post(CBEAM, data=json.dumps({'method': 'barstatus', 'params': [newbarstate], 'id': 0}))

