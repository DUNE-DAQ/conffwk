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

namespace dunedaq::conffwk::python {

extern void
register_conffwk(py::module&);

extern void
register_conffwkobject(py::module&);

PYBIND11_MODULE(_daq_conffwk_py, m)
{

  m.doc() = "Python interface to the conffwk package"; // optional module docstring

  register_conffwk(m);
  register_conffwkobject(m);
}

} // namespace dunedaq::conffwk::python
