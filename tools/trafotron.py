#!/usr/bin/env python3

import time
from threading import Thread
import copy
import json
import requests
from http.server import HTTPServer, BaseHTTPRequestHandler
from pylibcerebrum.serial_mux import SerialMux

CBEAM			= 'http://10.0.1.27:4254/rpc/'
HYPERBLAST		= 'http://10.0.1.23:1337/'
CLOCC			= 'http://c-lab-lock.cbrp3.c-base.org:1337/'
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

oldre, oldge, oldgn = None,None,None
def ampel(re,ge,gn):
	global oldre, oldge, oldgn
	if re and ge and gn:
		gn = False
	# Ensure no more than two lights are on at the same time, even for very short periods of time
	if oldre != re:
		g.ampelrot.state  = re
	if oldge != ge:
		g.ampelgelb.state = ge
	if oldgn != gn:
		g.ampelgrün.state = gn
	oldre,oldge,oldgn = re,ge,gn

#HACK ctrl-c ctrl-p -ed from barstatus.py
barstatus = 'closed'
ampelstate = ((0,0,0), (0,0,0))
lastchange = time.time() - 180
def animate():
	global barstatus, lastchange, ampelstate
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
		ampel(*ampelstate[0])
		time.sleep(0.33)
		(g.digital3.pwm, g.digital5.pwm, g.digital6.pwm), (g.digital9.pwm, g.digital10.pwm, g.digital11.pwm) = l2, r2
		ampel(*ampelstate[1])
		time.sleep(0.66)

animator = Thread(target=animate)
animator.daemon = True
animator.start()


def sendstate(value):
	print('SENDING', value)
	requests.post(CBEAM, data=json.dumps({'method': 'trafotron', 'params': [value], 'id': 0}))

class AmpelHandler(BaseHTTPRequestHandler):
	def do_POST(self):
		global ampelstate
		self.send_response(200)
		self.end_headers()
		postlen = int(self.headers['Content-Length'])
		postdata = str(self.rfile.read(postlen), 'utf-8')
		data = json.loads(postdata)
		method = data.get('method')
		if method == 'ampel':
			p = data.get('params')
			if type(p[0]) is list:
				(r1,y1,g1),(r2,y2,g2) = p
				r1,y1,g1 = bool(r1), bool(y1), bool(g1)
				r2,y2,g2 = bool(r2), bool(y2), bool(g2)
				ampelstate = ((r1,y1,g1),(r2,y2,g2))
			elif type(p[0]) is int and len(p) == 1:
				a,b = (bool(p[0]&32), bool(p[0]&16), bool(p[0]&8)), (bool(p[0]&4), bool(p[0]&2), bool(p[0]&1))
				ampelstate = a,b
			else:
				r,y,g = p
				r,y,g = bool(r), bool(y), bool(g)
				ampelstate = (r,y,g), (r,y,g)

HOST, PORT = '', 1337
server = HTTPServer((HOST, PORT), AmpelHandler)
t = Thread(target=server.serve_forever)
t.daemon = True
t.start()

time.sleep(2)

# Enable pull-up on Arduino analog pin 4
g.analog4.state = 1
oldval = -2*SEND_THRESHOLD
oldbarstate = None
newbarstate = None
while True:
	val = sum([ g.analog5.analog for i in range(AVG_SAMPLES)])/AVG_SAMPLES
	if g.analog4.state:
		newbarstate = 'closed'
	else:
		newbarstate = 'open'
	if newbarstate != oldbarstate:
		oldbarstate = newbarstate

		#comm with animation thread
		barstatus = newbarstate
		lastchange = time.time()

		requests.post(HYPERBLAST, data=json.dumps({'method': 'barstatus', 'params': [newbarstate], 'id': 0}))
		requests.post(CBEAM, data=json.dumps({'method': 'barstatus', 'params': [newbarstate], 'id': 0}))
		requests.post(CLOCC, data=json.dumps({'method': 'barstatus', 'params': [newbarstate], 'id': 0}))

