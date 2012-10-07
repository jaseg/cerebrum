
import serial
import json
import struct
import pylzma

class Ganglion:
	def __init__(self, config=None, configfile=None, proxies=None):
		if isinstance(config, dict):
			self.config = config
			self.proxies = proxies
			return
		if configfile is not None:
			cf = open(configfile, 'r')
			config = cf.readlines()
			cf.close()
		parsed_config = json.JSONDecoder().decode('\n'.join(config))
		#FIXME add sanity check(s) here or just let it fail?
		self.config = parsed_config["log"]
		self.proxies = {}
		for dname, dconf in parsed_config["dev"].items():
			self.proxies[dname] = SerialProxy(dconf)
	
	def children(self):
		self.config.keys()
   
	def __getattr__(self, name):
		if name is "config":
			raise AttributeError(name)
		if name in self.config:
			node = self.config[name]
			if isinstance(node, str):
				[dname, endpoint] = node.split('.')
				return self.proxies[dname].__getattr__(endpoint)
			else:
				return Ganglion(config=node, proxies=self.proxies)
		raise AttributeError(name)

	def __setattr__(self, name, value):
		if not name is "config" and name in self.config:
			[dname, endpoint] = self.config[name].split('.')
			proxies[dname].set_value(endpoint, value)
		else:
			self.__dict__[name] = value

class SerialProxy:
	# NOTE: the device config is *not* the stuff from the config "dev" section but
	#read from the device. It can also be found in that [devicename].config.json
	#file created by the code generator
	def __init__(self, config, ser=None):
		self.config = None
		if ser is None:
			#FIXME HACK to et the initialization go smooth despite the __*__ special functions and "config" not yet being set
			self.ser = serial.Serial(config["serial"], config["baudrate"])
			self.config = self.read_config()
		else:
			self.config = config
			self.ser = ser

	def read_config(self):
		self.ser.write(b'\\#\x00\x00\x00\x00')
		(clen,) = struct.unpack(">H", self.ser.read(2))
		cbytes = self.ser.read(clen)
		self.ser.read(2) #read and ignore the not-yet-crc
		return json.JSONDecoder().decode(str(pylzma.decompress(cbytes), "UTF-8"))
	
	@property
	def properties(self):
		return self.config["properties"].keys()

	@property
	def functions(self):
		return self.config["functions"].keys()

	def callfunc(self, fid, argsfmt, args, retfmt):
		cmd = b'\\#' + struct.pack(">HH", fid, struct.calcsize(argsfmt)) + struct.pack(argsfmt, *args) + b'\x00\x00'
		print("COMMAND: {}".format(cmd))
		self.ser.write(cmd)
		(clen,) = struct.unpack(">H", self.ser.read(2))
		cbytes = self.ser.read(clen)
		self.ser.read(2) #read and ignore the not-yet-crc
		if not clen == struct.calcsize(retfmt):
			#FIXME error handling
			print("Length mismatch: {} != {}".format(clen, struct.calcsize(retfmt)))
			return None
		return struct.unpack(retfmt, cbytes)


	def __setattr__(self, name, value):
		if not name is "config":
			if self.config and "properties" in self.config and name in self.config["properties"]:
				var = self.config["properties"][name]
				return self.callfunc(var["id"]+1, var["fmt"], value, "")
		self.__dict__[name] = value

	def __getattr__(self, name):
		if not self.config:
			raise AttributeError(name)

		if "members" in self.config and name in self.config["members"]:
			return SerialProxy(self.config["members"][name], self.ser)

		if "properties" in self.config and name in self.config["properties"]:
			var = self.config["properties"][name]
			return self.callfunc(var["id"], "", None, var["fmt"])

		if "functions" in self.config and  name in self.config["functions"]:
			fun = self.config["functions"][name]
			def proxy_method(*args):
				return self.callfunc(fun["id"], fun["args"], args, fun["returns"])
			return proxy_method

