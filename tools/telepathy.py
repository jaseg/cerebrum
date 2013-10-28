#!/usr/bin/env python3
""" Make things flicker. """

import time, random, sys
import re
import json
import argparse
from collections import defaultdict
import requests
import socket
from pylibcerebrum.serial_mux import SerialMux
from flask import Flask, jsonify
from flask_swagger import ApiParameter, SwaggerApiRegistry

# === Config ===
# Interval to wait after a failed HTTP/JSONRPC request
HOSTNAME = socket.getfqdn(socket.gethostname())

PORT = '/dev/serial/by-id/usb-Arduino__www.arduino.cc__0043_64936333037351C0B032-if00'
BAUDRATE = 115200

# === Flask infrastructure ===

app = Flask("cerebrum")
registry = SwaggerApiRegistry(app, baseurl='http://'+HOSTNAME+'/cerebrum')

# === Ganglion/Flask REST adapters ===

ARGTYPES = {
		'c': ('integer', int),
		'b': ('integer', int),
		'B': ('integer', int),
		'?': ('boolean', bool),
		'h': ('integer', int),
		'H': ('integer', int),
		'i': ('integer', int),
		'I': ('integer', int),
		'l': ('integer', int),
		'L': ('integer', int),
		'q': ('integer', int),
		'Q': ('integer', int),
		'f': ('float', float),
		'd': ('double', float),
		's': ('string', str),
		'p': ('string', str),
		'P': ('string', str)
		}
def structGenerator(fmt):
	global ARGTYPES
	if fmt[0] in list('@=<>!'):
		fmt = fmt[1:]
	for match in re.finditer('(\d*)([cbB?hHiIlLqQfdspP])', fmt):
		count, t = match.groups()
		if count:
			swaggertype, convfunc = ARGTYPES[t]
			yield '{}[{}]'.format(swaggertype, count), convfunc
		else:
			yield ARGTYPES[t]

class GenericGanglionAdapter:
	def __init__(self, parent, g):
		self.parent = parent
		self.g = g
		self.name = g.name
		print('Registering', self.name, self.base_url)
		self.members = [GANGLION_ADAPTERS[m.type](self, m) for _, m in g.members.items()]

		@registry.register(self.base_url, endpoint=self.base_url)
		def api_list():
			"""List this node's children, functions and properties."""
			nonlocal self
			return jsonify({'members': {m.name: {'url': m.base_url} for m in self.members},
					'functions': list(self.g.functions.keys()),
					'properties': list(self.g.properties.keys())})

		for prop, (_,fmt,_) in self.g.properties.items():
			swaggertypes, convfuncs = zip(*list(structGenerator(fmt)))
			uri = self.base_url +'/'+ prop
			@registry.register(uri, endpoint=uri, method='GET')
			@registry.register(uri, endpoint=uri,
					method='POST',
					parameters=[
						ApiParameter(name='param'+str(i),
							description='Auto-generated parameter narf',
							required=True,
							dataType=t,
							paramType='query',
							allowMultiple=False)
						for i, t in enumerate(swaggertypes)])
			def wrapper():
				"""Auto-generated wrapper function. Will complain when served garbage."""
				nonlocal self, prop
				if request.method == 'GET':
					return jsonify(getattr(self.g, prop.name))
				elif request.method == 'POST':
					data = request.json
					assert list(data) and len(data) == len(convfuncs)
					setattr(self.g, prop.name, [f(v) for f, v in zip(convfuncs, data)])

	@property
	def base_url(self):
		return self.parent.base_url +'/'+ self.name

GANGLION_ADAPTERS = defaultdict(lambda: GenericGanglionAdapter)
def register_ganglion_adapter(cls, ganglion_type):
	global GANGLION_ADAPTERS
	GANGLION_ADAPTERS[ganglion_type] = cls
	return cls

class GanglionRestAdapter:
	def __init__(self, g):
		self.name = g.config.get('name') or g.config.get('node_id')
		#Hardware discovery
		self.members = [GANGLION_ADAPTERS[m.type](self, g) for m in g]
	
	@property
	def base_url(self):
		return '/cerebrum'

# === Cerebrum serial port setup ===
s = SerialMux(PORT, BAUDRATE)
time.sleep(1)
print('Discovering cerebrum devices')
results = []
while not results:
	results = s.discover()
print('Opening devices')
adapters = [GanglionRestAdapter(s.open(i)) for _,i in results]

if __name__ == '__main__':
	app.debug = True
	app.run()

