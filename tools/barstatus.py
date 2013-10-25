#!/usr/bin/env python3

import time
import copy
import json
import requests
import time
from threading import Thread
from pylibcerebrum.serial_mux import SerialMux
from http.server import HTTPServer, BaseHTTPRequestHandler

PORT		= '/dev/serial/by-id/usb-Arduino__www.arduino.cc__Arduino_Uno_649383232313512181A1-if00'
BAUDRATE	= 115200

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
for io in [g.digital3, g.digital5, g.digital6, g.digital9, g.digital10, g.digital11]:
	io.direction = 1
	io.pwm_enabled = 1
print('starting event loop')

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

class BarstatusHandler(BaseHTTPRequestHandler):
	def do_POST(self):
		self.send_response(200)
		self.end_headers()
		postlen = int(self.headers['Content-Length'])
		postdata = str(self.rfile.read(postlen), 'utf-8')
		data = json.loads(postdata)
		method = data.get('method')
		if method == 'barstatus':
			[status,] = data.get('params')
			global barstatus
			barstatus = status
			global lastchange
			lastchange = time.time()

if __name__ == '__main__':
        HOST, PORT = '', 1337
        server = HTTPServer((HOST, PORT), BarstatusHandler)
        server.serve_forever()

