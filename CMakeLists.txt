cmake_minimum_required(VERSION 3.12)
project(config VERSION 0.0.0)

find_package(daq-cmake REQUIRED)

daq_setup_environment()

find_package(Boost COMPONENTS program_options REQUIRED)
find_package(ers REQUIRED)
find_package(logging REQUIRED)

remove_definitions(-DERS_NO_DEBUG)

daq_add_library(*.cpp LINK_LIBRARIES ers::ers logging::logging)

daq_add_python_bindings(*.cpp LINK_LIBRARIES config)

daq_add_application(config_dump config_dump.cxx                    LINK_LIBRARIES config Boost::program_options)
daq_add_application(config_export_data config_export_data.cxx      LINK_LIBRARIES config Boost::program_options)
daq_add_application(config_export_schema config_export_schema.cxx  LINK_LIBRARIES config Boost::program_options)

daq_add_application(config_time_test config_time_test.cxx  	   TEST       LINK_LIBRARIES config)
daq_add_application(config_test_object config_test_object.cxx      TEST       LINK_LIBRARIES config)
daq_add_application(config_test_rw config_test_rw.cxx              TEST	      LINK_LIBRARIES config)

#tdaq_add_library(pyconfig src/python/config.cpp INCLUDE_DIRECTORIES PythonLibs LINK_LIBRARIES PRIVATE config Boost::python)

#target_compile_options(pyconfig PRIVATE -Wno-unused) 


#add_test(NAME config_check_cpp  COMMAND ${TDAQ_RUNNER} ${CMAKE_CURRENT_SOURCE_DIR}/test/test.sh ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR})

#add_test(NAME config_check_py   COMMAND ${TDAQ_RUNNER} ${CMAKE_CURRENT_SOURCE_DIR}/test/pytest.sh ${CMAKE_CURRENT_SOURCE_DIR})

#tdaq_add_test(NAME PyStyleTest POST_INSTALL COMMAND env PYTHONDONTWRITEBYTECODE=x ${CMAKE_CURRENT_SOURCE_DIR}/test/pystyle.sh WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

daq_install()
