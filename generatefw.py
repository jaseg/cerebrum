#!/usr/bin/env python3

import sys
import os
import json
import imp

desc = json.JSONDecoder().decode('\n'.join(sys.stdin.readlines()))

if "dev" not in desc or "log" not in desc:
    exit(1)

for devicename in desc["dev"].keys
    dev = desc["dev"][devicename]
    typepath = os.path.realpath(__file__) + dev["type"]
    if os.path.exists(typepath):
        #known type
        ctx = imp.load_source("generate", typepath)
        ctx.generate(dev)
        ctx.commit(dev)
    else:
        print("Cannot find a handler for endpoint type " + dev["type"])
        exit(1)

