#!/usr/bin/env python
import serial
import sys
import time
ser = serial.Serial("/dev/ttyACM0", 57600)

#val = 0;
#while True:
#    ser.write('a')
#    ser.write(chr(0))
#    ser.write(chr(val))
#    ser.write('\n')
#    val+=1
#    val%=256
#    time.sleep(0.25);

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
