
import serial
import json
import struct
import pylzma
import time

class TimeoutException(Exception):
	pass

class Ganglion:
	# NOTE: the device config is *not* the stuff from the config "dev" section but
	#read from the device. It can also be found in that [devicename].config.json
	#file created by the code generator
	def __init__(self, device=None, baudrate=115200, config=None, ser=None):
		#FIXME HACK to et the initialization go smooth despite the __*__ special functions and "config" not yet being set
		self._config = None
		if ser is None:
			self._ser = serial.Serial(port=device, baudrate=baudrate, timeout=0.5)
			#Trust me, without the following two lines it *wont* *work*. Fuck serial ports.
			self._ser.setXonXoff(True)
			self._ser.setXonXoff(False)
			self._opened_ser = self._ser
			i=0
			while True:
				try:
					self._config = self.read_config()
					break
				except TimeoutException:
					print("Timeout")
					pass
				i += 1
				if i > 20:
					raise serial.serialutil.SerialException('Could not connect, giving up after 20 tries')
		else:
			self._config = config
			self._ser = ser
	
	def __del__(self):
		self.close()

	def close(self):
		try:
			self._opened_ser.close()
		except AttributeError:
			pass

	def type(self):
		return self._config["type"]

	def __iter__(self):
		return GanglionIter(self)

	@property
	def members(self):
		if "members" in self._config:
			return list(self._config["members"].keys())
		return []

	@property
	def properties(self):
		if "properties" in self._config:
			return list(self._config["properties"].keys())
		return []

	@property
	def functions(self):
		if "functions" in self._config:
			return list(self._config["functions"].keys())
		return []

	def my_ser_read(self, n):
		data = self._ser.read(n)
		if len(data) is not n:
			raise TimeoutException()
		return data

	def read_config(self):
		#since arglen==0 the crc is not needed
		self._ser.write(b'\\#\x00\x00\x00\x00')
		(clen,) = struct.unpack(">H", self.my_ser_read(2))
		cbytes = self.my_ser_read(clen)
		self.my_ser_read(2) #read and ignore the not-yet-crc
		return json.JSONDecoder().decode(str(pylzma.decompress(cbytes), "UTF-8"))

	def callfunc(self, fid, argsfmt, args, retfmt):
		cmd = b'\\#' + struct.pack(">HH", fid, struct.calcsize(argsfmt)) + struct.pack(argsfmt, *args) + (b'\x00\x00' if struct.calcsize(argsfmt) > 0 else b'')
		self._ser.write(cmd)
		(clen,) = struct.unpack(">H", self.my_ser_read(2))
		cbytes = self.my_ser_read(clen)
		self.my_ser_read(2) #read and ignore the not-yet-crc
		if clen is not struct.calcsize(retfmt):
			#FIXME error handling
			print("Length mismatch: {} != {}".format(clen, struct.calcsize(retfmt)))
			return None
		return struct.unpack(retfmt, cbytes)

	def __dir__(self):
		return self.members + self.properties + self.functions + list(self.__dict__.keys())

	def __setattr__(self, name, value):
		if name is not "_config":
			if self._config and "properties" in self._config and name in self._config["properties"]:
				var = self._config["properties"][name]
				return self.callfunc(var["id"]+1, var["fmt"], value, "")
		self.__dict__[name] = value

	def __getattr__(self, name):
		if not self._config:
			raise AttributeError(name)

		if "members" in self._config and name in self._config["members"]:
			return Ganglion(config=self._config["members"][name], ser=self._ser)

		if "properties" in self._config and name in self._config["properties"]:
			var = self._config["properties"][name]
			return self.callfunc(var["id"], "", None, var["fmt"])

		if "functions" in self._config and  name in self._config["functions"]:
			fun = self._config["functions"][name]
			def proxy_method(*args):
				return self.callfunc(fun["id"], fun.get("args", ""), args, fun.get("returns", ""))
			return proxy_method

		raise AttributeError(name)

class GanglionIter:

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

