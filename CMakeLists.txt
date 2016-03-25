cmake_minimum_required(VERSION 2.8.11)

get_filename_component(DEPENDS_DIR ${CMAKE_CURRENT_LIST_DIR}/depends ABSOLUTE)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${DEPENDS_DIR}/cmake/Modules)
include(${DEPENDS_DIR}/cmake/ImportCustomLibrary.cmake)
include(${DEPENDS_DIR}/cmake/configure_boost_env.cmake)

project(HTTPServer)

configure_boost_env()
find_package(Boost 1.55.0 REQUIRED)

set(target HTTPServer)
set(${target}_HEADERS
    src/Server.h
	src/util.h
)
set(${target}_SOURCES
    src/main.cpp
	src/Server.cpp
	src/util.cpp
)
set(${target}_RESOURCES
    #main.cpp
)

add_executable(${target}
    ${${target}_HEADERS}
    ${${target}_SOURCES}
    ${${target}_RESOURCES}
)

import_custom_library(${target} ThreadPool)
