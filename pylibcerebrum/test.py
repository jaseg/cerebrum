from pylibcerebrum.pylibcerebrum import Ganglion
import unittest
import serial
import generator

class TestGanglion(generator.TestCommStuff):

    def setUp(self):
        super(TestGanglion, self).setUp()

    def test_connect(self):
        fs = FakeSerial()
        #fs.inp += b'\x00K]\x00\x00\x80\x00\x00=\x88\x8a\xc6\x94S\x90\x86\xa6c}%:\xbbAj\x14L\xd9\x1a\xae\x93n\r\x10\x83E1\xba]j\xdeG\xb1\xba\xa6[:\xa2\xb9\x8eR~#\xb9\x84%\xa0#q\x87\x17[\xd6\xcdA)J{\xab*\xf7\x96%\xff\xfa\x12g\x00'
        fs.inp += b'\x00\x3F{"version":0.17,"builddate":"2012-05-23 23:42:17","members":{}}'

        g = Ganglion(ser=fs)
        g.config = g._read_config()
        self.assertEqual(fs.out, b'\\#\x00\x00\x00\x00', 'The ganglion sent garbage trying to read the device config.')
        self.assert_('version' in g.config, 'The ganglion has an invalid config without a version attribute')
        self.assertEqual(g.config['version'], 0.17, 'The ganglion\'s config\'s version attribute is wrong')
        self.assert_('members' in g.config, 'The ganglion has an invalid config without a \'members\' attribute')
        self.assertEqual(g.config['members'], {}, 'The ganglion\'s config\'s memers attribute is not an empty hash')

    def test_simple_callback_invocation(self):
        fs = FakeSerial()
        g = Ganglion(ser=fs)
        g._config = {'version': 0.17, 'builddate': '2012-05-23 23:42:17', 'members': {"foo": {"type": "test", "functions": {"callback": {"id": 1}}}}}
        #put the device's response into the input of the ganglion
        fs.inp += b'\x00\x00\x00\x00'
        #access the attribute
        g.foo.callback()
        self.assertEqual(fs.out, b'\\#\x00\x01\x00\x00', 'Somehow pylibcerebrum sent a wrong command to the device.')

    def test_complex_callback_invocation(self):
        fs = FakeSerial()
        g = Ganglion(ser=fs)
        g._config = {'version': 0.17, 'builddate': '2012-05-23 23:42:17', 'members': {"foo": {"type": "test", "functions": {"callback": {"id": 1, "args": "3B", "returns": "3B"}}}}}
        #put the device's response into the input of the ganglion
        fs.inp += b'\x00\x03ABC'
        #access the attribute
        foo = g.foo.callback(0x44, 0x45, 0x46)
        self.assertEqual(fs.out, b'\\#\x00\x01\x00\x03DEF', 'Somehow pylibcerebrum sent a wrong command to the device.')
        self.assertEqual(foo, (0x41, 0x42, 0x43), 'Somehow a device response was decoded wrong.')

    def test_attribute_read(self):
        fs = FakeSerial()
        g = Ganglion(ser=fs)
        g._config = {'version': 0.17, 'builddate': '2012-05-23 23:42:17', 'members': {"foo": {"type": "test", "properties": {"prop": {"fmt": "B", "id": 1, "size": 1}}}}}
        #put the device's response into the input of the ganglion
        fs.inp += b'\x00\x01\x41'
        #access the attribute
        (foo,) = g.foo.prop
        self.assertEqual(fs.out, b'\\#\x00\x01\x00\x00', 'Somehow pylibcerebrum sent a wrong command to the device.')
        self.assertEqual(foo, 0x41, 'Somehow a device response was decoded wrong.')

    def test_attribute_read_long(self):
        #This reads a rather large attribute (>64 bytes) in order to catch problems with multipart callbacks.
        fs = FakeSerial()
        g = Ganglion(ser=fs)
        g._config = {'version': 0.17, 'builddate': '2012-05-23 23:42:17', 'members': {"foo": {"type": "test", "properties": {"prop": {"fmt": "65B", "id": 1, "size": 65}}}}}
        #put the device's response into the input of the ganglion
        fs.inp += b'\x00\x41AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA'
        #access the attribute
        foo = g.foo.prop
        self.assertEqual(fs.out, b'\\#\x00\x01\x00\x00', 'Somehow pylibcerebrum sent a wrong command to the device.')
        self.assertEqual(foo, (0x41,)*65, 'Somehow a device response was decoded wrong.')

    def test_attribute_write(self):
        fs = FakeSerial()
        g = Ganglion(ser=fs)
        g._config = {'version': 0.17, 'builddate': '2012-05-23 23:42:17', 'members': {"foo": {"type": "test", "properties": {"prop": {"fmt": "B", "id": 1, "size": 1}}}}}
        #put the device's response into the input of the ganglion
        fs.inp += b'\x00\x00'
        #access the attribute
        g.foo.prop = 0x41
        self.assertEqual(fs.out, b'\\#\x00\x02\x00\x01\x41', 'Somehow pylibcerebrum sent a wrong command to the device.')

    def test_attribute_write_long(self):
        #This writes a rather large attribute (>64 bytes) in order to catch problems with multipart callbacks.
        fs = FakeSerial()
        g = Ganglion(ser=fs)
        g._config = {'version': 0.17, 'builddate': '2012-05-23 23:42:17', 'members': {"foo": {"type": "test", "properties": {"prop": {"fmt": "257B", "id": 1, "size": 0x101}}}}}
        #put the device's response into the input of the ganglion
        fs.inp += b'\x00\x00'
        #access the attribute
        g.foo.prop = (0x41,)*0x101
        self.assertEqual(fs.out, b'\\#\x00\x02\x01\x01'+b'A'*0x101, 'Somehow pylibcerebrum sent a wrong command to the device.')
        pass

    def test_attribute_forbidden_write(self):
        fs = FakeSerial()
        g = Ganglion(ser=fs)
        g._config = {'version': 0.17, 'builddate': '2012-05-23 23:42:17', 'members': {"foo": {"type": "test", "properties": {"prop": {"fmt": "B", "id": 1, "size": 1, "access": "r"}}}}}
        #access the attribute
        with self.assertRaises(TypeError, msg="prop is a read-only property"):
            g.foo.prop = 0x41

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

