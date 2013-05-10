
import serial
import threading
import struct
from pylibcerebrum.ganglion import Ganglion
from pylibcerebrum.timeout_exception import TimeoutException

class SerialMux(object):

	def __init__(self, device=None, baudrate=115200, ser=None):
		s = ser or LockableSerial(port=device, baudrate=baudrate, timeout=1)
		#Trust me, without the following two lines it *wont* *work*. Fuck serial ports.
		s.setXonXoff(True)
		s.setXonXoff(False)
		s.setDTR(True)
		s.setDTR(False)
		self.ser = s
	
	def open(self, node_id):
		""" Open a Ganglion by node ID """
		return Ganglion(node_id, ser=self.ser)

	def discover(self, mask=0, address=0, found=[]):
		""" Discover all node IDs connected to the bus
		
			Note: You do not need to set any of the arguments. These are used for the recursive discovery process.
		"""
		for a in [address, 1<<mask | address]:
			if self._send_probe(a, 15-mask):
				if(mask < 15):
					self.discover(mask+1, a, found)
				else:
					found.append(a)
		return found

	def _send_probe(self, address, mask):
		#print('Discovery: address', address, 'mask', mask)
		with self.ser as s:
			s.write(b'\\#\xFF\xFF' + struct.pack('>HH', address, mask))
			timeout_tmp = s.timeout
			s.timeout = 0.005
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

