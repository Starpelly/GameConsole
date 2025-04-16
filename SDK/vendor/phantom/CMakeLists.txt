cmake_minimum_required (VERSION 3.20.2)
project(phantom)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

set(
    PHANTOM_SOURCES
    phantomcolor.h
    phantomstyle.h
    phantomtweak.h

    phantomcolor.cpp
    phantomstyle.cpp
)

add_library(phantom STATIC ${PHANTOM_SOURCES})

target_include_directories(phantom PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(phantom PUBLIC Qt${QT_VERSION_MAJOR}::Widgets)
