cmake_minimum_required(VERSION 2.8.11)

project(HTTPServer)

set(target HTTPServer)
set(${target}_HEADERS
    
)
set(${target}_SOURCES
    main.cpp
)
set(${target}_RESOURCES
    main.cpp
)

add_executable(${target}
    ${${target}_HEADERS}
    ${${target}_SOURCES}
    ${${target}_RESOURCES}
)