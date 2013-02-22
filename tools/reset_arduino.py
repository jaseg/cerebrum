#!/usr/bin/env python3
import serial
import sys
ser = serial.Serial(sys.argv[1], 115200)
ser.setDTR(True)
ser.setDTR(False)
del ser
