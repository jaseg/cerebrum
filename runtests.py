#!/usr/bin/env python3

import generator
import pylibcerebrum.test
import unittest

suite = unittest.TestLoader().loadTestsFromModule(generator)
#suite.addTest(unittest.TestLoader().loadTestsFromModule(pylibcerebrum.test))
unittest.TextTestRunner(verbosity=2).run(suite)

