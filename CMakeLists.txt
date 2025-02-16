cmake_minimum_required(VERSION 3.16)
project(influxdb)

set(CMAKE_INCLUDE_CURRENT_DIR "YES")
set(CMAKE_AUTOMOC "YES")
set(CMAKE_AUTOUIC "YES")

if(NOT CMAKE_DEBUG_POSTFIX)
  set(CMAKE_DEBUG_POSTFIX d)
endif()

find_package(Qt6 COMPONENTS Network REQUIRED)
find_package(Qt6 COMPONENTS Widgets REQUIRED)

qt_add_library(influxdb SHARED
        influxdb.cpp
        influxdb.h
)

target_link_libraries(influxdb Qt6::Network)
target_link_libraries(influxdb Qt6::Widgets)

target_include_directories(influxdb INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

set_target_properties(influxdb
    PROPERTIES
        LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/$<CONFIG>/lib
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/$<CONFIG>/bin
)


