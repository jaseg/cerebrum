from pylibcerebrum.pylibcerebrum import Ganglion
import unittest

import serial

import generator

class TestGanglion(generator.TestCommStuff):

    def setUp(self):
        super(TestGanglion, self).setUp()

    def test_connect(self):
        fs = FakeSerial()
        #fs.inp += b'\x00K]\x00\x00\x80\x00\x00=\x88\x8a\xc6\x94S\x90\x86\xa6c}%:\xbbAj\x14L\xd9\x1a\xae\x93n\r\x10\x83E1\xba]j\xdeG\xb1\xba\xa6[:\xa2\xb9\x8eR~#\xb9\x84%\xa0#q\x87\x17[\xd6\xcdA)J{\xab*\xf7\x96%\xff\xfa\x12g\x00\x00\x00'
        fs.inp += b'\x00\x3F{"version":0.17,"builddate":"2012-05-23 23:42:17","members":{}}\x00\x00'

        g = Ganglion(ser=fs)
        g.config = g._read_config()
        self.assertEqual(fs.out, b'\\#\x00\x00\x00\x00', 'The ganglion sent garbage trying to read the device config.')
        self.assert_('version' in g.config, 'The ganglion has an invalid config without a version attribute')
        self.assertEqual(g.config['version'], 0.17, 'The ganglion\'s config\'s version attribute is wrong')
        self.assert_('members' in g.config, 'The ganglion has an invalid config without a \'members\' attribute')
        self.assertEqual(g.config['members'], {}, 'The ganglion\'s config\'s memers attribute is not an empty hash')

    def test_attribute_read(self):
        fs = FakeSerial()
        g = Ganglion(ser=fs)
        g.config = {'version': 0.17, 'builddate': '2012-05-23 23:42:17', 'members': {"led": {"type": "test", "properties": {"test": {"fmt": "B", "id": 1, "size": 1}}}}}
        fs.inp += b'\x00\x00\x00\x00'
        #FIXME

    def test_attribute_write(self):
        #FIXME
        pass

    def test_attribute_forbidden_write(self):
        #FIXME
        pass

    def test_function_invocation(self):
        #FIXME
        pass

class FakeSerial:
    
    def __init__(self):
        self.out = b''
        self.inp = b''

    def read(self, n):
        r = self.inp[0:n]
        self.inp = self.inp[n:]
        return r

    def write(self, bs):
        if not isinstance(bs, bytes):
            raise ArgumentError('FakeSerial.write only accepts -bytes-')
        self.out += bs

