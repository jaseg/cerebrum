#!/usr/bin/env python
import serial
import sys
ser = serial.Serial("/dev/ttyACM0", 57600)

while True:
    print ser.read()

while True:
    val = int(sys.stdin.readline())
    print 'a',
    print ord(chr(0)),
    print ord(chr(val))
    ser.write('a')
    ser.write(chr(0))
    ser.write(chr(val))
    ser.write('\n')
    print ser.readline()
