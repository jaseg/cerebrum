#!/usr/bin/env python3

import avr.generate
import pylibcerebrum.test
import unittest

suite = unittest.TestLoader().loadTestsFromModule(avr.generate)
suite.addTest(unittest.TestLoader().loadTestsFromModule(pylibcerebrum.test))
unittest.TextTestRunner(verbosity=2).run(suite)

