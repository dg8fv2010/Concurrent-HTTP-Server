cmake_minimum_required(VERSION 2.8.11)

project(HTTPServer)

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