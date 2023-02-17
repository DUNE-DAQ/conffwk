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

namespace dunedaq::oksdbinterfaces::python {

extern void
register_oksdbinterfaces(py::module&);

extern void
register_oksdbinterfacesobject(py::module&);

PYBIND11_MODULE(_daq_oksdbinterfaces_py, m)
{

  m.doc() = "Python interface to the oksdbinterfaces package"; // optional module docstring

  register_oksdbinterfaces(m);
  register_oksdbinterfacesobject(m);
}

} // namespace dunedaq::oksdbinterfaces::python
