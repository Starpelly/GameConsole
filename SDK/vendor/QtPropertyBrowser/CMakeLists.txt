cmake_minimum_required (VERSION 3.20.2)
project(QtPropertyBrowser)

# set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

include_directories(src)

set(SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")

file(GLOB_RECURSE SOURCES
    "${SOURCE_DIR}/*.cpp"
    "${SOURCE_DIR}/*.h"
)

#macro(AddFileDirectory dir)
#    file(GLOB SRC ${dir}/*.*)

#    set(LOCAL_SRC_ALL ${LOCAL_SRC_ALL} ${SRC})
#endmacro()

#AddFileDirectory(src)

add_library(QtPropertyBrowser STATIC ${SOURCES})

target_include_directories(QtPropertyBrowser PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)

target_link_libraries(QtPropertyBrowser PUBLIC Qt${QT_VERSION_MAJOR}::Widgets)
