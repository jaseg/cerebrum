#!/usr/bin/env python3
#
#Copyright (C) 2012 jaseg <s@jaseg.de>
#
#This program is free software; you can redistribute it and/or
#modify it under the terms of the GNU General Public License
#version 3 as published by the Free Software Foundation.

import serial
import json
import struct
import pylzma
import time
"""Call RPC functions on serially connected devices over the Cerebrum protocol."""

class TimeoutException(Exception):
	pass

class Ganglion:
	"""Proxy class for calling remote methods on hardware connected through a serial port using the Cerebrum protocol"""

	# NOTE: the device config is *not* the stuff from the config "dev" section but
	#read from the device. It can also be found in that [devicename].config.json
	#file created by the code generator
	def __init__(self, device=None, baudrate=115200, config=None, ser=None):
		"""Ganglion constructor

		Keyword arguments:
		device -- the device file to connect to
		baudrate -- the baudrate to use (default 115200)
		The other keyword arguments are for internal use only.

		"""
		#FIXME HACK to et the initialization go smooth despite the __*__ special functions and "config" not yet being set
		self._config = None
		if ser is None:
			assert(config is None)
			self._ser = serial.Serial(port=device, baudrate=baudrate, timeout=0.5)
			#Trust me, without the following two lines it *wont* *work*. Fuck serial ports.
			self._ser.setXonXoff(True)
			self._ser.setXonXoff(False)
			self._opened_ser = self._ser
			i=0
			while True:
				try:
					self._config = self._read_config()
					break
				except TimeoutException:
					print("Timeout")
					pass
				i += 1
				if i > 20:
					raise serial.serialutil.SerialException('Could not connect, giving up after 20 tries')
		else:
			assert(device is None)
			self._config = config
			self._ser = ser
	
	def __del__(self):
		self.close()
	
	def __exit__(self, exception_type, exception_val, trace):
		self.close()

	def close(self):
		"""Close the serial port."""
		try:
			self._opened_ser.close()
		except AttributeError:
			pass

	def type(self):
		"""Return the cerebrum type of this node.
		
		If this method returns None, that means the node has no type as it is the
		case in pure group nodes. For leaf nodes the leaf type is returned, the
		root node returns the device type (e.g. "avr")
		"""
		return self._config.get("type")

	def __iter__(self):
		"""Construct an iterator to iterate over *all* (direct or not) child nodes of this node."""
		return GanglionIter(self)

	@property
	def members(self):
		"""Return a list of child node names of this node."""
		if "members" in self._config:
			return list(self._config["members"].keys())
		return []

	@property
	def properties(self):
		"""Return a list of property names of this node."""
		if "properties" in self._config:
			return list(self._config["properties"].keys())
		return []

	@property
	def functions(self):
		"""Return a list of function names of this node."""
		if "functions" in self._config:
			return list(self._config["functions"].keys())
		return []

	def _my_ser_read(self, n):
		"""Read n bytes from the serial device and raise a TimeoutException in case of a timeout."""
		data = self._ser.read(n)
		if len(data) is not n:
			raise TimeoutException()
		return data

	def _read_config(self):
		"""Fetch the device configuration descriptor from the device."""
		#Is the crc necessary here?
		self._ser.write(b'\\#\x00\x00\x00\x00')
		(clen,) = struct.unpack(">H", self._my_ser_read(2))
		cbytes = self._my_ser_read(clen)
		self._my_ser_read(2) #read and ignore the not-yet-crc
		return json.JSONDecoder().decode(str(pylzma.decompress(cbytes), "UTF-8"))

	def _callfunc(self, fid, argsfmt, args, retfmt):
		"""Call a function on the device by id, directly passing argument/return format parameters."""
		cmd = b'\\#' + struct.pack(">HH", fid, struct.calcsize(argsfmt)) + struct.pack(argsfmt, *args) + (b'\x00\x00' if struct.calcsize(argsfmt) > 0 else b'')
		self._ser.write(cmd)
		#payload length
		(clen,) = struct.unpack(">H", self._my_ser_read(2))
		#payload data
		cbytes = self._my_ser_read(clen)
		#crc
		self._my_ser_read(2) #read and ignore the not-yet-crc
		if clen is not struct.calcsize(retfmt):
			#CAUTION! This error is thrown not because the user supplied a wrong value but because the device answered in an unexpected manner.
			#FIXME raise an error here or let the whole operation just fail in the following struct.unpack?
			raise ArgumentError("Device response format problem: Length mismatch: {} != {}".format(clen, struct.calcsize(retfmt)))
		return struct.unpack(retfmt, cbytes)

	def __dir__(self):
		"""Get a list of all attributes of this object. This includes virtual Cerebrum stuff like members, properties and functions."""
		return self.members + self.properties + self.functions + list(self.__dict__.keys())

	def __setattr__(self, name, value):
		"""Magic method to set an attribute. This one even handles remote Cerebrum properties."""
		if name is not "_config": #Guard against all too infinite recursion
			#check if the name is a known property
			if self._config and "properties" in self._config and name in self._config["properties"]:
				#call the property's Cerebrum setter function
				var = self._config["properties"][name]
				#FIXME add access checking
				return self._callfunc(var["id"]+1, var["fmt"], [value], "")
		#if the above code falls through, do a normal __dict__ lookup.
		self.__dict__[name] = value

	def __getattr__(self, name):
		"""Magic method to get an attribute of this object, considering Cerebrum members, properties and functions.

		At this point a hierarchy is imposed upon the members/properties/functions that is not present in the implementation:

		Between a member, a property and a function of the same name the member will be preferred over the property and the property will be preferred over the function. If you should manage to make device have such colliding names, consider using _callfunc(...) directly.

		"""
		if not self._config: #Guard against all too infinite recursion
			raise AttributeError(name)

		if "members" in self._config and name in self._config["members"]:
			return Ganglion(config=self._config["members"][name], ser=self._ser)

		if "properties" in self._config and name in self._config["properties"]:
			var = self._config["properties"][name]
			return self._callfunc(var["id"], "", (), var["fmt"])

		if "functions" in self._config and  name in self._config["functions"]:
			fun = self._config["functions"][name]
			def proxy_method(*args):
				return self._callfunc(fun["id"], fun.get("args", ""), args, fun.get("returns", ""))
			return proxy_method

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

