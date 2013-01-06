#!/usr/bin/env python3

import generator
import pylibcerebrum.test
import unittest

#The generator will be tested as a part of pylibcerebrum due to the inheritance of the test classes.
suite = unittest.TestLoader().loadTestsFromModule(pylibcerebrum.test)
unittest.TextTestRunner(verbosity=2).run(suite)

