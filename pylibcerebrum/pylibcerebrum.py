
import serial
import json
import struct

class Ganglion
    def __init__(self, config=None, configfile=None, proxies=None):
		if isInstance(config, dict):
			self.config = config
			self.proxies = proxies
			return
        if configfile is not None:
            cf = open(configfile, 'r')
            config = cf.readlines()
            cf.close()
        self.config = json.JSONDecoder().decode('\n'.join(config))
		#FIXME add sanity check(s) here or just let it fail?
		self.proxies = {}
		for dname, dconf in self.config["dev"].items():
			self.proxies[dname] = SerialProxy(dname, dconf)
   
    def __getattr__(self, name):
		if name in self.config:
			node = self.config[name]
			if isInstance(node, str):
				[dname, endpoint] = node.split('.')
				return proxies[dname].__getattr__(endpoint)
			else:
				return Ganglion(config=node, proxies=self.proxies)

	def __setattr__(self, name, value):
		if name in self.config:
			[dname, endpoint] = self.config[name].split('.')
			proxies[dname].set_value(endpoint, value)
		else:
			self.__dict__[name] = value

class SerialProxy:
	# NOTE: the device config is *not* the stuff from the config "dev" section but
	#read from the device. It can also be found in that [devicename].config.json
	#file created by the code generator
    def __init__(self, device_config):
		self.ser = serial.Serial(device_config["serial"], device_config["baudrate"]))
		self.config = read_config()
	
	def read_config(self):
		ser.write("\\#\0\0\0")
		clen = stuct.unpack("h", ser.read(2))
		cbytes = ser.read(clen)
		self.config = json.JSONDecoder().decode(cbytes)

	def __setattr__(self, name, value):
		if name in self.config["properties"]:
			var = self.config["properties"][name]
			cmd="\\#" + struct.pack("hh", var["id"]+1, struct.calcsize(var["fmt"])) + struct.pack(var["fmt"], value)
			ser.write(cmd)
			clen = stuct.unpack("h", ser.read(2))
			cbytes = ser.read(clen)
			if not clen == 0:
				#FIXME error handling
				pass
		else:
			self.__dict__[name] = value

	def __getattr__(self, name):
		if name in self.config["properties"]:
			var = self.config["properties"][name]
			cmd="\\#" + struct.pack("hh", var["id"], 0)
			ser.write(cmd)
			clen = stuct.unpack("h", ser.read(2))
			cbytes = ser.read(clen)
			if not clen == struct.calcsize(var["fmt"])):
				#FIXME error handling
				return None
			return struct.unpack(var["fmt"], cbytes)
		elif name in self.config["functions"]:
			fun = self.config["functions"][name]

			def proxy(args):
				cmd="\\#" + struct.pack("hh", fun["id"], struct.calcsize(fun["args"])) + struct.pack(fun["args"], args)
				ser.write(cmd)
				clen = stuct.unpack("h", ser.read(2))
				cbytes = ser.read(clen)
				if not clen == struct.calcsize(fun["returns"])):
					#FIXME error handling
					return None
				return struct.unpack(fun["returns"], cbytes)

			return proxy
		else:
			self.__dict__[name] = value

