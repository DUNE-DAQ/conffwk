#!/usr/bin/env python
# vim: set fileencoding=utf-8 :
# DUNE DAQ modification notice:
# This file has been modified from the original ATLAS config source for the DUNE DAQ project.
# Fork baseline commit: 67a24e731 (2022-10-27).
# Renamed since fork: yes (from python/tests/test_all.py to scripts/test_all.py).

# Created by Andre Anjos <andre.dos.anjos@cern.ch>
# Wed 24 Oct 2007 01:55:04 PM CEST

"""Run all unittests integrated
"""

from test_configuration import *
from test_conffwkobject import *
from test_dal import *

if __name__ == "__main__":
    import sys
    sys.argv.append('-v')
    unittest.main()
