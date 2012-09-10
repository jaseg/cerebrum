#!/usr/bin/env python
import serial
ser = serial.Serial("/dev/ttyACM0", 57600)
while(True):
    print ser.readline(),
