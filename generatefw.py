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

parser = argparse.ArgumentParser(description='Cerebrum firmware generator', epilog='To program the device upon creating the firmware image, supply either -p or -s.')
parser.add_argument('template', type=argparse.FileType('r'), default='-', help='The build template .json file')
parser.add_argument('-p', '--port', type=str, help='The tty where the device may be found to be programmed')
parser.add_argument('-b', '--baudrate', type=int, help='The baud rate of the device')
parser.add_argument('-s', '--usbserial', type=str, help='The USB serial number by which the device can be identified in order to be programmed')
parser.add_argument('-n', '--buildname', type=str, help='An optional name for the build. This is used to name the build config files.')
args = parser.parse_args()

desc = json.JSONDecoder().decode('\n'.join(args.template.readlines()))
st = datetime.datetime.utcnow().timetuple()
builddate = str(datetime.datetime(st[0], st[1], st[2], st[3], st[4], st[5]))

typepath = os.path.join(os.path.dirname(__file__), desc["type"], "generate.py")
print(builddate)
buildsource = 'stdin' if args.template.name is '-' else args.template.name
print('Generating firmware from ', buildsource)
print('Using generator ', typepath)
ctx = imp.load_source("generate", typepath)
(deviceconfig, output) = ctx.generate(desc, args.buildname, builddate)
with open(os.path.join(os.path.dirname(__file__), "builds", builddate + "-" + args.buildname if args.buildname else os.path.splitext(os.path.basename(buildsource))[0] + ".config.json"), "w") as f:
	f.write(json.JSONEncoder(indent=4).encode(deviceconfig))
	print('Wrote build config to ', f.name)

if args.port or args.usbserial:
	print('Programming device')
	ctx.commit(args)

