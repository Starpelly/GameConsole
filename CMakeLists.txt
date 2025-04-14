cmake_minimum_required(VERSION 3.15)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(Soulcast)


set(SOULCAST_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(SOULCAST_VENDOR_DIR "${CMAKE_CURRENT_SOURCE_DIR}/vendor")

file(GLOB_RECURSE SOULCAST_FILES
    ${SOULCAST_SOURCE_DIR}/*.cpp
    ${SOULCAST_SOURCE_DIR}/*.h
    ${SOULCAST_SOURCE_DIR}/*.hpp
)

add_executable(Soulcast ${SOULCAST_FILES})
include_directories(${SOULCAST_SOURCE_DIR})

#--------------------------------------------------------------------
# Windows Libraries
#--------------------------------------------------------------------
target_link_libraries(Soulcast PRIVATE "netapi32.lib")
target_link_libraries(Soulcast PRIVATE "Version.lib")
target_link_libraries(Soulcast PRIVATE "user32.lib")
target_link_libraries(Soulcast PRIVATE "gdi32.lib")
target_link_libraries(Soulcast PRIVATE "windowsapp.lib")
target_link_libraries(Soulcast PRIVATE "shell32.lib")

#--------------------------------------------------------------------
# SDL3
#--------------------------------------------------------------------
set(SDL_TEST_LIBRARY OFF)
set(SDL_STATIC ON)
set(SDL_SHARED OFF)

target_link_libraries(Soulcast PRIVATE SDL3-static)

add_subdirectory("vendor/SDL3")
include_directories("vendor/SDL3/include")

#--------------------------------------------------------------------
# Folder structuring in Visual Studio
#--------------------------------------------------------------------
if(MSVC_IDE)
	set_target_properties(SDL3-static PROPERTIES FOLDER "Vendor/SDL3")
	set_target_properties(SDL_uclibc PROPERTIES FOLDER "Vendor/SDL3")

	foreach(source IN LISTS SOULCAST_FILES)
		get_filename_component(source_path "${source}" PATH)
		string(REPLACE "${CMAKE_CURRENT_SOURCE_DIR}" "" relative_source_path "${source_path}")
		string(REPLACE "/" "\\" source_path_msvc "${relative_source_path}")
		source_group("${source_path_msvc}" FILES "${source}")
    endforeach()
endif()