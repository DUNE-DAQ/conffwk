/**
 * @file config.cpp
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#include "config/Configuration.hpp"
#include "config/ConfigObject.hpp"

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/stl_bind.h>

#include <list>
#include <string>

namespace py = pybind11;

namespace dunedaq::config::python {

void
register_config(py::module& m)
{

  py::class_<Configuration>(m, "_Configuration")
    .def(py::init<>())
    .def(py::init<std::string>())
    .def("add_include",
	 &Configuration::add_include, "Add include file to existing database.", py::arg("db_name"), py::arg("include"))
    .def("commit",
	 &Configuration::commit, "Commit database changes.", py::arg("log_message") = "")
    .def("create_db",
	 static_cast<void (Configuration::*)(const std::string&, const std::list<std::string>&)>(&Configuration::create), "Create a database from a list of files", py::arg("db_name"), py::arg("includes"))
    .def("create_obj",
	 py::overload_cast<const std::string&, const std::string&, const std::string&>(&Configuration::create_and_return_obj), "Create new object by class name and object id.", py::arg("at"), py::arg("class_name"), py::arg("id"))
    .def("create_obj",
	 py::overload_cast<const ConfigObject&, const std::string&, const std::string&>(&Configuration::create_and_return_obj), "Create new object by class name and object id.", py::arg("at"), py::arg("class_name"), py::arg("id"))
    .def("destroy_obj",
	 &Configuration::destroy_obj, "The method tries to destroy given object.", py::arg("object"))
    .def("get_includes",
	 &Configuration::return_includes, "Returns list of files included by given database.", py::arg("db_name"))
    .def("get_obj",
     	 &Configuration::get_obj, "Create a configuration object containing the desired entity from the database", py::arg("class_name"), py::arg("id"))
    .def("get_objs",
	 &Configuration::get_objs, "Create a list of configuration objects of a given class from the database", py::arg("class_name"), py::arg("query") = "")
    .def("load",
	 &Configuration::load, "Load database according to the name.", py::arg("db_name"))
    .def("loaded",
	 &Configuration::loaded, "Check if database is correctly loaded.")
    .def("remove_include",
	 &Configuration::remove_include, "Remove include file.", py::arg("db_name"), py::arg("include"))
    .def("test_object",
	 &Configuration::test_object, "Test the existence of the object",py::arg("class_name"), py::arg("id"), py::arg("rlevel") = 0, py::arg("rclasses") = nullptr)
    .def("unload",
	 &Configuration::unload, "Unload previously-loaded database")
    ;
}

} // namespace dunedaq::config::python
