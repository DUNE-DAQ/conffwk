cmake_minimum_required(VERSION 3.12)
project(conffwk VERSION 1.2.0)

find_package(daq-cmake REQUIRED)

daq_setup_environment()

find_package(Boost COMPONENTS program_options REQUIRED)
find_package(ers REQUIRED)
find_package(logging REQUIRED)

remove_definitions(-DERS_NO_DEBUG)

daq_add_library(*.cpp LINK_LIBRARIES ers::ers logging::logging)

daq_add_python_bindings(*.cpp LINK_LIBRARIES conffwk)

daq_add_application(config_dump config_dump.cxx                    LINK_LIBRARIES conffwk Boost::program_options)
daq_add_application(config_export_data config_export_data.cxx      LINK_LIBRARIES conffwk Boost::program_options)
daq_add_application(config_export_schema config_export_schema.cxx  LINK_LIBRARIES conffwk Boost::program_options)

daq_add_application(config_time_test config_time_test.cxx  	   TEST       LINK_LIBRARIES conffwk)
daq_add_application(config_test_object config_test_object.cxx      TEST       LINK_LIBRARIES conffwk)
daq_add_application(config_test_rw config_test_rw.cxx              TEST	      LINK_LIBRARIES conffwk)


daq_install()
