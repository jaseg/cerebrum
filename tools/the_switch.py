#!/usr/bin/env python

from http.server import HTTPServer, BaseHTTPRequestHandler
import json
import subprocess

play_process = None

def start_playing():
	global play_process
	if play_process is None:
		play_process = subprocess.Popen(['mplayer', '/var/switch/Smile Song.mp3'], stdout=subprocess.PIPE, stderr=subprocess.PIPE)

def stop_playing():
	global play_process
	if play_process is not None:
		play_process.terminate()
		play_process = None

class SwitchHandler(BaseHTTPRequestHandler):
	def do_POST(self):
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

if __name__ == '__main__':
	HOST, PORT = '127.0.0.1', 1337
	server = HTTPServer((HOST, PORT), SwitchHandler)
	server.serve_forever()
