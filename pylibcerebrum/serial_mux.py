
import serial
import threading
import struct
from pylibcerebrum.ganglion import Ganglion, escape
from pylibcerebrum.timeout_exception import TimeoutException

MAC_LEN = 64

class SerialMux(object):

	def __init__(self, device=None, baudrate=115200, ser=None, timeout=1):
		s = ser or LockableSerial(port=device, baudrate=baudrate, timeout=timeout)
		#Trust me, without the following two lines it *wont* *work*. Fuck serial ports.
		s.setXonXoff(True)
		s.setXonXoff(False)
		#Reset Arduinos
		s.setDTR(True)
		s.setDTR(False)
		self.ser = s
	
	def open(self, node_id):
		""" Open a Ganglion by node ID """
		return Ganglion(node_id, ser=self.ser)

	def discover(self, mask=0, mac=0, found=None):
		""" Discover all node IDs connected to the bus
		
			Note: You do not need to set any of the arguments. These are used for the recursive discovery process.
		"""
		if found is None:
			found = []
		for a in [mac, 1<<mask | mac]:
			next_address = len(found)
			if self._send_probe(a, MAC_LEN-1-mask, next_address):
				if(mask < MAC_LEN-1):
					self.discover(mask+1, a, found)
				else:
					found.append((a, next_address))
		return found

	def _send_probe(self, mac, mask, next_address):
		#print('Discovery: mac', mac, 'mask', mask)
		with self.ser as s:
			s.write(b'\\#\xFF\xFF' + escape(struct.pack('>HHQ', next_address, mask, mac)))
			timeout_tmp = s.timeout
			s.timeout = 0.05
			try:
				s.read(1)
				s.timeout = timeout_tmp
				return True
			except TimeoutException:
				s.timeout = timeout_tmp
				return False
	
	def __del__(self):
		self.ser.close()

class LockableSerial(serial.Serial):
	def __init__(self, *args, **kwargs):
		super(serial.Serial, self).__init__(*args, **kwargs)
		self.lock = threading.RLock()

	def __enter__(self):
		self.lock.__enter__()
		return self
	
	def __exit__(self, *args):
		self.lock.__exit__(*args)
	
	def read(self, n):
		"""Read n bytes from the serial device and raise a TimeoutException in case of a timeout."""
		data = serial.Serial.read(self, n)
		if len(data) != n:
			raise TimeoutException('Read {} bytes trying to read {}'.format(len(data), n))
		return data

