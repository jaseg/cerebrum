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

desc = json.JSONDecoder().decode('\n'.join(sys.stdin.readlines()))
builddate = str(datetime.datetime.now())

# I think it is better to just let it throw an error
#if "dev" not in desc or "log" not in desc:
#	 exit(1)

for devicename, dev in desc["dev"].items():
	dev = desc["dev"][devicename]
	typepath = os.path.join(os.path.dirname(__file__), dev["type"], "generate.py")
#	 if not os.path.exists(typepath):
#		 print("Cannot find a handler for endpoint type " + dev["type"])
#		 exit(1)
	#known type
	print(typepath)
	ctx = imp.load_source("generate", typepath)
	deviceconfig = ctx.generate(dev, devicename, builddate)
	with open(os.path.join(os.path.dirname(__file__), "builds", builddate + "-" + devicename + ".config.json"), "w") as f:
		f.write(json.JSONEncoder(indent=4).encode(deviceconfig))
	ctx.commit(dev)

