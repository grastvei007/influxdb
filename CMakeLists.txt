cmake_minimum_required(VERSION 3.16)
project(influxdb)

#include_directories($ENV{JUNE_ROOT})
#link_directories($ENV{DEV_LIBS})

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

add_custom_command(TARGET influxdb
                   POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:tagsystem> $ENV{DEV_LIBS} )


