#!/usr/bin/env python
import serial
ser = serial.Serial("/dev/ttyUSB0", 57600)
while(True):
    print ser.readline(),
