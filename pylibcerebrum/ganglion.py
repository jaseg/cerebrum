#!/usr/bin/env python3
#
#Copyright (C) 2012 jaseg <s@jaseg.de>
#
#This program is free software; you can redistribute it and/or
#modify it under the terms of the GNU General Public License
#version 3 as published by the Free Software Foundation.

import json
import struct
try:
	import lzma
except:
	import pylzma as lzma
import time
import serial
from pylibcerebrum.NotifyList import NotifyList
from pylibcerebrum.timeout_exception import TimeoutException

escape = lambda s: s.replace(b'\\', b'\\\\')

"""Call RPC functions on serially connected devices over the Cerebrum protocol."""

class Ganglion(object):
	"""Proxy class for calling remote methods on hardware connected through a serial port using the Cerebrum protocol"""

	# NOTE: the device config is *not* the stuff from the config "dev" section but
	#read from the device. It can also be found in that [devicename].config.json
	#file created by the code generator
	def __init__(self, node_id, jsonconfig=None, ser=None, name=''):
		"""Ganglion constructor

		Keyword arguments:
		device -- the device file to connect to
		baudrate -- the baudrate to use (default 115200)
		The other keyword arguments are for internal use only.

		"""
		object.__setattr__(self, '_ser', ser)
		object.__setattr__(self, 'node_id', node_id)
		if not jsonconfig:
			# get a config
			i=0
			while True:
				try:
					jsonconfig = self._read_config()
					time.sleep(0.1)
					break
				except TimeoutException as e:
					print('Timeout', e)
				except ValueError as e:
					print('That device threw some nasty ValueError\'ing JSON!', e)
				i += 1
				if i > 20:
					raise serial.serialutil.SerialException('Could not connect, giving up after 20 tries')
		if not name:
			name = jsonconfig.get('name')
		object.__setattr__(self, 'name', name)
		# populate the object
		object.__setattr__(self, 'members', {})
		for name, member in jsonconfig.get('members', {}).items():
			self.members[name] = Ganglion(node_id, jsonconfig=member, ser=self._ser, name=name)
		object.__setattr__(self, 'properties', {})
		for name, prop in jsonconfig.get('properties', {}).items():
			self.properties[name] = (prop['id'], prop['fmt'], prop.get('access', 'rw'))
		object.__setattr__(self, 'functions', {})
		for name, func in jsonconfig.get('functions', {}).items():
			def proxy_method(*args):
				return self._callfunc(func["id"], func.get("args", ""), args, func.get("returns", ""))
			self.functions[name] = proxy_method
		object.__setattr__(self, 'type', jsonconfig.get('type', None))
		object.__setattr__(self, 'config', { k: v for k,v in jsonconfig.items() if not k in ['members', 'properties', 'functions'] })
	
	def __iter__(self):
		"""Construct an iterator to iterate over *all* (direct or not) child nodes of this node."""
		return GanglionIter(self)

	def _read_config(self):
		"""Fetch the device configuration descriptor from the device."""
		with self._ser as s:
			s.write(b'\\#' + escape(struct.pack(">H", self.node_id)) + b'\x00\x00\x00\x00')
			(clen,) = struct.unpack(">H", s.read(2))
			cbytes = s.read(clen)
			#decide whether cbytes contains lzma or json depending on the first byte (which is used as a magic here)
			if cbytes[0] is ord('#'):
				return json.JSONDecoder().decode(str(lzma.decompress(cbytes[1:]), "utf-8"))
			else:
				return json.JSONDecoder().decode(str(cbytes, "utf-8"))
	
	def _callfunc(self, fid, argsfmt, args, retfmt):
		"""Call a function on the device by id, directly passing argument/return format parameters."""
		# Make a list out of the arguments if they are none
		#print('calling function No. {}, args({}) {}, returning {}'.format(fid, argsfmt, args, retfmt))
		if not (isinstance(args, tuple) or isinstance(args, list)):
			args = [args]
		with self._ser as s:
			# Send the encoded data
			cmd = b'\\#' + escape(struct.pack(">HHH", self.node_id, fid, struct.calcsize(argsfmt)) + struct.pack(argsfmt, *args))
			s.write(cmd)
			# payload length
			(clen,) = struct.unpack(">H", s.read(2))
			# payload data
			cbytes = s.read(clen)
			if clen != struct.calcsize(retfmt):
				# CAUTION! This error is thrown not because the user supplied a wrong value but because the device answered in an unexpected manner.
				# FIXME raise an error here or let the whole operation just fail in the following struct.unpack?
				raise AttributeError("Device response format problem: Length mismatch: {} != {}".format(clen, struct.calcsize(retfmt)))
			rv = struct.unpack(retfmt, cbytes)
			# Try to interpret the return value in a useful manner
			if len(rv) == 0:
				return None
			elif len(rv) == 1:
				return rv[0]
			else:
				return list(rv)

	def __dir__(self):
		"""Get a list of all attributes of this object. This includes virtual Cerebrum stuff like members, properties and functions."""
		return list(self.members.keys()) + list(self.properties.keys()) + list(self.functions.keys()) + list(self.__dict__.keys())
	
	# Only now add the setattr magic method so it does not interfere with the above code
	def __setattr__(self, name, value):
		"""Magic method to set an attribute. This one even handles remote Cerebrum properties."""
		#check if the name is a known property
		if name in self.properties:
			#call the property's Cerebrum setter function
			varid, varfmt, access = self.properties[name]
			if not "w" in access:
				raise TypeError("{} is a read-only property".format(name))
			return self._callfunc(varid+1, varfmt, value, "")
		#if the above code falls through, do a normal __dict__ lookup.
		self.__dict__[name] = value


	def __getattr__(self, name):
		"""Magic method to get an attribute of this object, considering Cerebrum members, properties and functions.

		At this point a hierarchy is imposed upon the members/properties/functions that is not present in the implementation:

		Between a member, a property and a function of the same name the member will be preferred over the property and the property will be preferred over the function. If you should manage to make device have such colliding names, consider using _callfunc(...) directly.

		"""
		if name in self.members:
			return self.members[name]

		if name in self.properties:
			def cb(newx):
				self.__setattr__(name, newx)
			varid, varfmt, access = self.properties[name]
			rv = self._callfunc(varid, "", (), varfmt)
			# If the return value is a list, construct an auto-updating thingy from it.
			if isinstance(rv, list):
				return NotifyList(rv, callbacks=[cb])
			else:
				return rv

		if name in self.functions:
			return self.functions[name]

		#If all of the above falls through...
		raise AttributeError(name)

class GanglionIter:
	"""Iterator class for ganglions that recursively iterates over all (direct or indirect) child nodes of a given Ganglion"""

	def __init__(self, g):
		self.g = g
		self.keyiter = g.members.__iter__()
		self.miter = None

	def __iter__(self):
		return self

	def __next__(self):
		try:
			return self.miter.__next__()
		except StopIteration:
			pass
		except AttributeError:
			pass
		foo = self.g.__getattr__(self.keyiter.__next__())
		self.miter = foo.__iter__()
		return foo

