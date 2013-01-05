#!/usr/bin/env python3
#
#Copyright (C) 2012 jaseg <s@jaseg.de>
#
#This program is free software; you can redistribute it and/or
#modify it under the terms of the GNU General Public License
#version 3 as published by the Free Software Foundation.

import sys
import os
import json
import imp
import datetime
import argparse
import generator
"""Generate firmware for Cerebrum devices according to a json-formatted device configuration passed on the command line."""

# Parse arguments
parser = argparse.ArgumentParser(description='Generate firmware for Cerebrum devices according to a json-formatted device configuration passed on the command line.', epilog='To program the device upon creating the firmware image, supply either -p or -s.')
parser.add_argument('template', type=argparse.FileType('r'), default='-', help='The build template .json file')
parser.add_argument('device', type=argparse.FileType('r'), help='The device type of this build. For available types, have a look at the "devices" directory.')
parser.add_argument('-p', '--port', type=str, help='The tty where the device may be found to be programmed')
parser.add_argument('-b', '--baudrate', type=int, help='The baud rate of the device')
parser.add_argument('-s', '--usbserial', type=str, help='The USB serial number by which the device can be identified in order to be programmed')
parser.add_argument('-n', '--buildname', type=str, help='An optional name for the build. This is used to name the build config files.')
args = parser.parse_args()

# Decode json device descriptor and device config and initialize build variables
desc = json.JSONDecoder().decode('\n'.join(args.template.readlines()))
device = json.JSONDecoder().decode('\n'.join(args.device.readlines()))
st = datetime.datetime.utcnow().timetuple()
builddate = str(datetime.datetime(st[0], st[1], st[2], st[3], st[4], st[5]))
buildsource = 'stdin' if args.template.name is '-' else args.template.name
print(builddate)
print('Generating firmware from ', buildsource)

# Import code generator module
build_path = os.path.join(os.path.dirname(__file__), desc["type"])

# Generate code and write generated build config
# FIXME there are two different but similar things called "build config" here.
buildconfig = generator.generate(desc, device, build_path, builddate)
with open(os.path.join(os.path.dirname(__file__), "builds", builddate + "-" + args.buildname if args.buildname else os.path.splitext(os.path.basename(buildsource))[0] + ".config.json"), "w") as f:
	f.write(json.JSONEncoder(indent=4).encode(buildconfig))
	print('Wrote build config to ', f.name)

# Flash the device if requested
if args.port or args.usbserial:
	print('Programming device')
	generator.commit(device, device['type'], args)

# ???

# PROFIT!!!

