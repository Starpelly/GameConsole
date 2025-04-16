cmake_minimum_required(VERSION 3.15)
project(spng C)

set(SPNG_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src/spng")

set(SPNG_SOURCES
	"${SPNG_SOURCE_DIR}/spng.c"
	"${SPNG_SOURCE_DIR}/spng.h"
	"${SPNG_SOURCE_DIR}/miniz.c"
	"${SPNG_SOURCE_DIR}/miniz.h"
)

add_library(spng STATIC ${SPNG_SOURCES})
target_include_directories(spng PRIVATE ${SPNG_SOURCE_DIR})
target_compile_definitions(spng PUBLIC SPNG_USE_MINIZ)
target_compile_definitions(spng PUBLIC SPNG_STATIC)