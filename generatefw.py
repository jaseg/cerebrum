#!/usr/bin/env python3

import sys
import os
import json
import imp

desc = json.JSONDecoder().decode('\n'.join(sys.stdin.readlines()))

# I think it is better to just let it throw an error
#if "dev" not in desc or "log" not in desc:
#    exit(1)

for devicename, dev in desc["dev"].items():
    dev = desc["dev"][devicename]
    typepath = os.path.join(os.path.dirname(__file__), dev["type"], "generate.py")
#    if not os.path.exists(typepath):
#        print("Cannot find a handler for endpoint type " + dev["type"])
#        exit(1)
    #known type
    print(typepath)
    ctx = imp.load_source("generate", typepath)
    deviceconfig = ctx.generate(dev, devicename)
    with open(os.path.join(os.path.dirname(__file__), "builds", devicename + '.config.json'), 'w') as f:
        f.write(json.JSONEncoder(indent=4).encode(deviceconfig))
    ctx.commit(dev)

