#!/bin/bash

eval $(get_lcg_env --prepend --python --ld --path pycodestyle)
pycodestyle python/conffwk/*.py python/tests/*.py

