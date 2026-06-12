# DUNE DAQ modification notice:
# This file has been modified from the original ATLAS config source for the DUNE DAQ project.
# Fork baseline commit: 67a24e731 (2022-10-27).
# Renamed since fork: yes (from python/config/__init__.py to python/conffwk/__init__.py).

from __future__ import absolute_import
from .Configuration import Configuration
from .ConfigObject import ConfigObject
from . import dal
from . import schema


def updated_dals():
    return dal.DalBase.updated()


updated_dals.__doc__ = dal.DalBase.updated.__doc__


def reset_updated_dals():
    dal.DalBase.reset_updated_list()


reset_updated_dals.__doc__ = dal.DalBase.reset_updated_list.__doc__
