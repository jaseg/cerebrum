#!/usr/bin/env python

from http.server import HTTPServer, BaseHTTPRequestHandler
import json
import subprocess
import random
import os

play_process = None
audiofiles = []

def start_playing():
	global play_process
	global audiofiles
	if play_process is None:
		#audiofiles = ['/var/switch/Smile Song.mp3'] * 2 + ['/var/switch/rick astley never gonna give you up.mp3'] + ['/var/switch/MitchiriNeko March.mp3'] * 7 + ['/var/switch/nyan-cat.mp3'] * 3 + ['/var/switch/Imperial_March.mp3'] * 3 + ['/var/switch/Drogenlied.mp3'] * 2
		#audiofile = random.choice(audiofiles)
		if len(audiofiles) < 1: 
			audiofiles = os.listdir('/var/switch')
		audiofile = '/var/switch/%s' % audiofiles.pop(random.randrange(len(audiofiles)))
		play_process = subprocess.Popen(['mplayer', '-really-quiet', audiofile])

def stop_playing():
	global play_process
	if play_process is not None:
		play_process.terminate()
		play_process = None

class SwitchHandler(BaseHTTPRequestHandler):
	def do_POST(self):
		global audiofiles
		self.send_response(200)
		self.end_headers()
		postlen = int(self.headers['Content-Length'])
		postdata = str(self.rfile.read(postlen), 'utf-8')
		data = json.loads(postdata)
		method = data.get('method')
		if method == 'cerebrumNotify':
			node, io, state = data.get('params')
			if node == 'nerdctrl' and io == '/the_switch':
				if state == 0:
					start_playing()
				else:
					stop_playing()
		elif method == 'start_playing':
			start_playing()
		elif method == 'stop_playing':
			stop_playing()
		elif method == 'ls':
			self.wfile.write(json.dumps(audiofiles))
			self.wfile.close()

if __name__ == '__main__':
	HOST, PORT = '127.0.0.1', 1337
	server = HTTPServer((HOST, PORT), SwitchHandler)
	server.serve_forever()
