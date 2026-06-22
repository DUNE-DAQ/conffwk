#!/bin/bash
# DUNE DAQ modification notice:
# This file has been modified from the original ATLAS config source for the DUNE DAQ project.
# Fork baseline commit: 67a24e731 (2022-10-27).
# Renamed since fork: yes (from test/pystyle.sh to scripts/conffwk_pystyle.sh).


eval $(get_lcg_env --prepend --python --ld --path pycodestyle)
pycodestyle python/conffwk/*.py python/tests/*.py

