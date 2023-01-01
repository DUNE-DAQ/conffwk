/**
 * @file module.cpp
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;

namespace dunedaq::config::python {

extern void
register_config(py::module&);

extern void
register_configobject(py::module&);

PYBIND11_MODULE(_daq_config_py, m)
{

  m.doc() = "Python interface to the config package"; // optional module docstring

  register_config(m);
  register_configobject(m);
}

} // namespace dunedaq::config::python
