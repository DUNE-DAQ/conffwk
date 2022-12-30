/**
 * @file config.cpp
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#include "config/Configuration.hpp"
#include "config/ConfigObject.hpp"
#include "config/Schema.hpp"
#include "config/set.hpp"

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/stl_bind.h>

#include <list>
#include <string>
#include <vector>

namespace py = pybind11;

namespace dunedaq::config::python {

void
register_mystruct(py::module& m) {
  py::class_<Configuration::MyStruct>(m, "MyStruct", py::dynamic_attr())
    .def(py::init<>())
    .def_readwrite("first_int", &Configuration::MyStruct::first_int)
    .def_readwrite("second_int", &Configuration::MyStruct::second_int)
    ;
}


void
register_config(py::module& m)
{

  // auto superclasses_in_python(const std::string& class_name, bool all) {
    
  //   //std::vector<dunedaq::config::fset> superclasses;

  //   const dunedaq::config::class_t& c = this->get_class_info(class_name, !all);

  //   //for (const auto& x : c.p_superclasses) {
  //   //superclasses.append(x);
  //   //}

  //   //return superclasses;
  //   return c.p_superclasses;
  // }



  py::class_<Configuration>(m, "_Configuration")
    .def(py::init<>())
    .def(py::init<std::string>())
    .def("add_include",
	 &Configuration::add_include, "Add include file to existing database.", py::arg("db_name"), py::arg("include"))
    .def("attributes",
	 &Configuration::attributes, "Get the properties of each attribute in a given class", py::arg("class_name"), py::arg("all"))
    .def("classes", 
	 &Configuration::classes_in_python, "Get the names of the superclasses for each class")
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
    .def("get_impl_param",
	 &Configuration::get_impl_param, "Get implementation plug-in parameter used to build config object")
    .def("get_impl_spec",
	 &Configuration::get_impl_spec, "Get implementation plug-in and its parameter used to build config object")
    .def("get_includes",
	 &Configuration::return_includes, "Returns list of files included by given database.", py::arg("db_name"))
    .def("get_obj",
     	 &Configuration::get_obj, "Create a configuration object containing the desired entity from the database", py::arg("class_name"), py::arg("id"))
    .def("get_objs",
	 &Configuration::get_objs, "Create a list of configuration objects of a given class from the database", py::arg("class_name"), py::arg("query") = "")
    .def("johnsfunction",
	 &Configuration::johnsfunction, "John testing what pybind11 is capable of", py::arg("first"), py::arg("second"))
    .def("load",
	 &Configuration::load, "Load database according to the name.", py::arg("db_name"))
    .def("loaded",
	 &Configuration::loaded, "Check if database is correctly loaded.")
    .def("remove_include",
	 &Configuration::remove_include, "Remove include file.", py::arg("db_name"), py::arg("include"))
    .def("relations",
	 &Configuration::relations, "Get the properties of each relation in a given class", py::arg("class_name"), py::arg("all"))
    .def("superclasses",
	 &Configuration::superclasses_in_python, "Get the superclasses of a single class", py::arg("class_name"), py::arg("all"))
    .def("subclasses",
	 &Configuration::subclasses_in_python, "Get the superclasses of a single class", py::arg("class_name"), py::arg("all"))
    .def("test_object",
    	 &Configuration::test_object, "Test the existence of the object", py::arg("class_name"), py::arg("id"), py::arg("rlevel"), py::arg("rclasses"))
    .def("unload",
	 &Configuration::unload, "Unload previously-loaded database")
    ;
}

void
register_configobject(py::module& m)
{

  py::class_<ConfigObject>(m, "_ConfigObject")
    .def(py::init<>())
    .def(py::init<const ConfigObject&>())
    .def("set_obj",
	 &ConfigObject::set_obj, "Set relationship single-value", py::arg("name"), py::arg("o"), py::arg("skip_non_null_check") = false)
    .def("set_objs",
	 &ConfigObject::set_objs, "Set relationship multi-value.", py::arg("name"), py::arg("o"), py::arg("skip_non_null_check") = false)
    //    .def("get_obj",
    //	 &ConfigObject::co_get_obj, "Get a copy of an object", py::arg("co"), py::arg("attrname"))
    .def("get_obj",
	 &ConfigObject::co_get_obj2, "Get a copy of an object", py::arg("attrname"))
    .def("get_objs",
	 &ConfigObject::get_list_in_python<ConfigObject>, "Getter function for a list", py::arg("co"), py::arg("attr"))
    .def("class_name",
	 &ConfigObject::class_name, "Return object's class name")
    .def("UID",
	 &ConfigObject::UID, "Return object identity")
    .def("full_name",
	 &ConfigObject::full_name, "Return full object name")
    .def("contained_in",
	 &ConfigObject::contained_in, "Return the name of the database file this object belongs to.")
    .def("rename",
	 &ConfigObject::rename, "Rename object", py::arg("new_id"))
    .def("get_string",
	 &ConfigObject::get_value_in_python<std::string>, "Simple getter function", py::arg("attr"))
    .def("get_bool",
	 &ConfigObject::get_value_in_python<bool>, "Simple getter function", py::arg("attr"))
    .def("get_s8",
	 &ConfigObject::get_value_in_python<int8_t>, "Simple getter function", py::arg("attr"))
    .def("get_u8",
	 &ConfigObject::get_value_in_python<uint8_t>, "Simple getter function", py::arg("attr"))
    .def("get_s16",
	 &ConfigObject::get_value_in_python<int16_t>, "Simple getter function", py::arg("attr"))
    .def("get_u16",
	 &ConfigObject::get_value_in_python<uint16_t>, "Simple getter function", py::arg("attr"))
    .def("get_s32",
	 &ConfigObject::get_value_in_python<int32_t>, "Simple getter function", py::arg("attr"))
    .def("get_u32",
	 &ConfigObject::get_value_in_python<uint32_t>, "Simple getter function", py::arg("attr"))
    .def("get_s64",
	 &ConfigObject::get_value_in_python<int64_t>, "Simple getter function", py::arg("attr"))
    .def("get_u64",
	 &ConfigObject::get_value_in_python<uint64_t>, "Simple getter function", py::arg("attr"))
    .def("get_float",
	 &ConfigObject::get_value_in_python<float>, "Simple getter function", py::arg("attr"))
    .def("get_double",
	 &ConfigObject::get_value_in_python<double>, "Simple getter function", py::arg("attr"))
    .def("get_string_vec",
	 &ConfigObject::get_list_in_python<std::string>, "Getter function for a list", py::arg("co"), py::arg("attr"))
    .def("get_bool_vec",
	 &ConfigObject::get_list_in_python<bool>, "Getter function for a list", py::arg("co"), py::arg("attr"))
    .def("get_s8_vec",
	 &ConfigObject::get_list_in_python<int8_t>, "Getter function for a list", py::arg("co"), py::arg("attr"))
    .def("get_u8_vec",
	 &ConfigObject::get_list_in_python<uint8_t>, "Getter function for a list", py::arg("co"), py::arg("attr"))
    .def("get_s16_vec",
	 &ConfigObject::get_list_in_python<int16_t>, "Getter function for a list", py::arg("co"), py::arg("attr"))
    .def("get_u16_vec",
	 &ConfigObject::get_list_in_python<uint16_t>, "Getter function for a list", py::arg("co"), py::arg("attr"))
    .def("get_s32_vec",
	 &ConfigObject::get_list_in_python<int32_t>, "Getter function for a list", py::arg("co"), py::arg("attr"))
    .def("get_u32_vec",
	 &ConfigObject::get_list_in_python<uint32_t>, "Getter function for a list", py::arg("co"), py::arg("attr"))
    .def("get_s64_vec",
	 &ConfigObject::get_list_in_python<int64_t>, "Getter function for a list", py::arg("co"), py::arg("attr"))
    .def("get_u64_vec",
	 &ConfigObject::get_list_in_python<uint64_t>, "Getter function for a list", py::arg("co"), py::arg("attr"))
    .def("get_float_vec",
	 &ConfigObject::get_list_in_python<float>, "Getter function for a list", py::arg("co"), py::arg("attr"))
    .def("get_double_vec",
	 &ConfigObject::get_list_in_python<double>, "Getter function for a list", py::arg("co"), py::arg("attr"))
    .def("get_objs",
	 &ConfigObject::get_list_in_python<ConfigObject>, "Getter function for a list", py::arg("co"), py::arg("attr"))
    .def("set_bool",
	 &ConfigObject::set_by_val<bool>, "Simple setter function", py::arg("name"), py::arg("value"))
    .def("set_s8", 
	 &ConfigObject::set_by_val<int8_t>, "Simple setter function", py::arg("name"), py::arg("value"))
    .def("set_u8", 
	 &ConfigObject::set_by_val<uint8_t>, "Simple setter function", py::arg("name"), py::arg("value"))
    .def("set_s16", 
	 &ConfigObject::set_by_val<int16_t>, "Simple setter function", py::arg("name"), py::arg("value"))
    .def("set_u16", 
	 &ConfigObject::set_by_val<uint16_t>, "Simple setter function", py::arg("name"), py::arg("value"))
    .def("set_s32", 
	 &ConfigObject::set_by_val<int32_t>, "Simple setter function", py::arg("name"), py::arg("value"))
    .def("set_u32", 
	 &ConfigObject::set_by_val<uint32_t>, "Simple setter function", py::arg("name"), py::arg("value"))
    .def("set_s64", 
	 &ConfigObject::set_by_val<int64_t>, "Simple setter function", py::arg("name"), py::arg("value"))
    .def("set_u64", 
	 &ConfigObject::set_by_val<uint64_t>, "Simple setter function", py::arg("name"), py::arg("value"))
    .def("set_float", 
	 &ConfigObject::set_by_val<float>, "Simple setter function", py::arg("name"), py::arg("value"))
    .def("set_double", 
	 &ConfigObject::set_by_val<double>, "Simple setter function", py::arg("name"), py::arg("value"))
    .def("set_date",
	 py::overload_cast<const std::string&, const std::string&>(&ConfigObject::set_date), "Set the date", py::arg("name"), py::arg("value"))
    .def("set_time",
	 py::overload_cast<const std::string&, const std::string&>(&ConfigObject::set_time), "Set the time", py::arg("name"), py::arg("value"))
    .def("set_class",
	 py::overload_cast<const std::string&, const std::string&>(&ConfigObject::set_class), "Set the class name", py::arg("name"), py::arg("value"))
    .def("set_enum",
	 py::overload_cast<const std::string&, const std::string&>(&ConfigObject::set_enum), "Set the enum", py::arg("name"), py::arg("value"))
    .def("set_string", 
	 &ConfigObject::set_by_val<std::string>, "Simple setter function", py::arg("name"), py::arg("value"))
    .def("set_bool_vec",
	 &ConfigObject::set_list_in_python<bool>, "Setter function for list", py::arg("co"), py::arg("attrname"), py::arg("l"))
    .def("set_s8_vec",
	 &ConfigObject::set_list_in_python<int8_t>, "Setter function for list", py::arg("co"), py::arg("attrname"), py::arg("l"))
    .def("set_u8_vec",
	 &ConfigObject::set_list_in_python<uint8_t>, "Setter function for list", py::arg("co"), py::arg("attrname"), py::arg("l"))
    .def("set_s16_vec",
	 &ConfigObject::set_list_in_python<int16_t>, "Setter function for list", py::arg("co"), py::arg("attrname"), py::arg("l"))
    .def("set_u16_vec",
	 &ConfigObject::set_list_in_python<uint16_t>, "Setter function for list", py::arg("co"), py::arg("attrname"), py::arg("l"))
    .def("set_s32_vec",
	 &ConfigObject::set_list_in_python<int32_t>, "Setter function for list", py::arg("co"), py::arg("attrname"), py::arg("l"))
    .def("set_u32_vec",
	 &ConfigObject::set_list_in_python<uint32_t>, "Setter function for list", py::arg("co"), py::arg("attrname"), py::arg("l"))
    .def("set_s64_vec",
	 &ConfigObject::set_list_in_python<int64_t>, "Setter function for list", py::arg("co"), py::arg("attrname"), py::arg("l"))
    .def("set_u64_vec",
	 &ConfigObject::set_list_in_python<uint64_t>, "Setter function for list", py::arg("co"), py::arg("attrname"), py::arg("l"))
    .def("set_float_vec",
	 &ConfigObject::set_list_in_python<float>, "Setter function for list", py::arg("co"), py::arg("attrname"), py::arg("l"))
    .def("set_double_vec",
	 &ConfigObject::set_list_in_python<double>, "Setter function for list", py::arg("co"), py::arg("attrname"), py::arg("l"))
    .def("set_date_vec",
	 &ConfigObject::set_date_list, "Set dates", py::arg("co"), py::arg("attrname"), py::arg("l"))
    .def("set_time_vec",
	 &ConfigObject::set_time_list, "Set times", py::arg("co"), py::arg("attrname"), py::arg("l"))
    .def("set_class_vec",
	 &ConfigObject::set_class_list, "Set class names", py::arg("co"), py::arg("attrname"), py::arg("l"))
    .def("set_enum_vec",
	 &ConfigObject::set_enum_list, "Set enums", py::arg("co"), py::arg("attrname"), py::arg("l"))
    .def("set_string_vec",
	 &ConfigObject::set_list_in_python<std::string>, "Set list of strings", py::arg("co"), py::arg("attrname"), py::arg("l"))
    ;
}


} // namespace dunedaq::config::python
