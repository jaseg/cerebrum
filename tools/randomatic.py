#!/usr/bin/env python3
""" Make things flicker. """

import time, random, sys
import json
import argparse
from collections import defaultdict
import requests
from pylibcerebrum.serial_mux import SerialMux

parser = argparse.ArgumentParser(description='Make things flicker.')
parser.add_argument('port', type=str, help='The TTY the target device is connected to')
parser.add_argument('-b', '--baudrate', type=str, default=115200, help='The TTYs baud rate')
parser.add_argument('-p', '--publish', type=str, help='Publish connected switches and inputs to the given JSONRPC server')
args = parser.parse_args()

# Interval to wait after a failed HTTP/JSONRPC request
RETRY_INTERVAL = 10.0

s = SerialMux(args.port, args.baudrate)
time.sleep(1)
print('Discovering cerebrum devices')
results = []
while not results:
	results = s.discover()
print('Opening first device')
g = s.open(0)
NODE_NAME = g.config.get('name') or g.config.get('node_id')
print('Initializing device, node name', NODE_NAME)

inputs       = [ m for m in g if m.type == 'matrix_input' ]
inputstates  = defaultdict(lambda: None)
led_matrices = [ m for m in g if m.type == 'led_matrix' ]
pwms		 = [ m for m in g if m.type == 'simple-io' and m.config.get('mode') == 'pwm' ]
print('Found', len(inputs), 'input matrices,', len(led_matrices), 'LED matrices and', len(pwms), 'PWM outputs.')

lamp_rand_data = [ [ [ random.randint(0, 1) for _ in range(28) ] for _ in led_matrices ] for _ in range(128) ]
pwm_rand_data  = [ x for _ in range(8) for x in [[ random.randint(0, 255) for _ in pwms ]]*16 ]


jsonrpc_retrytime = 0
def jsonrpc_notify(io, state):
	global jsonrpc_retrytime
	if time.time() > jsonrpc_retrytime:
		try:
			requests.post(args.publish, data=json.dumps({'method': 'cerebrumNotify', 'params': [NODE_NAME, io, state], 'id': 0}))
		except Exception as e:
			print('JSONRPC request failed, waiting', RETRY_INTERVAL, 'seconds:', e)
			jsonrpc_retrytime = time.time() + RETRY_INTERVAL


while True:
	for lamps, pwmvs in zip(lamp_rand_data, pwm_rand_data):
		for m, d in zip(led_matrices, lamps):
			m.buffer = d
		for m, d in zip(pwms, pwmvs):
			m.pwm = d

		for m in inputs:
			st = m.state
			for i, e in enumerate(m.config['mapping']):
				inputid = m.name+'/'+str(i)

				decstate = 0
				for bit in e[0]:
					decstate = (decstate<<1) | st[bit]

				maxval = 2**len(e[0])
				statemap = list(range(maxval))
				statemap[:len(e)-1] = e[1:]

				state = statemap[decstate]
				if inputstates[inputid] != state:
					inputstates[inputid] = state
					jsonrpc_notify(inputid, state)

		time.sleep(0.2)

