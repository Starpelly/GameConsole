cmake_minimum_required(VERSION 3.15)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(SoulcastEngine)

set(SOULCAST_ENGINE_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(SOULCAST_ENGINE_VENDOR_DIR ${CMAKE_CURRENT_SOURCE_DIR}/vendor)

file(GLOB_RECURSE SOULCAST_ENGINE_FILES
    ${SOULCAST_ENGINE_SOURCE_DIR}/*.cpp
    ${SOULCAST_ENGINE_SOURCE_DIR}/*.hpp
)

add_library(SoulcastEngine SHARED ${SOULCAST_ENGINE_FILES})
include_directories(${SOULCAST_ENGINE_SOURCE_DIR})

#--------------------------------------------------------------------
# Compile Definitions
#--------------------------------------------------------------------
target_compile_definitions(SoulcastEngine PRIVATE SOULCAST_BUILD_DLL)

#--------------------------------------------------------------------
# Windows Libraries
#--------------------------------------------------------------------
target_link_libraries(SoulcastEngine PRIVATE "netapi32.lib")
target_link_libraries(SoulcastEngine PRIVATE "Version.lib")
target_link_libraries(SoulcastEngine PRIVATE "user32.lib")
target_link_libraries(SoulcastEngine PRIVATE "gdi32.lib")
target_link_libraries(SoulcastEngine PRIVATE "windowsapp.lib")
target_link_libraries(SoulcastEngine PRIVATE "shell32.lib")

#--------------------------------------------------------------------
# SDL3
#--------------------------------------------------------------------
set(SDL_TEST_LIBRARY OFF)
set(SDL_STATIC ON)
set(SDL_SHARED OFF)

add_subdirectory(${SOULCAST_ENGINE_VENDOR_DIR}/SDL3)
target_include_directories(SoulcastEngine PRIVATE ${SOULCAST_ENGINE_VENDOR_DIR}/SDL3/include)

target_link_libraries(SoulcastEngine PRIVATE SDL3-static)

#--------------------------------------------------------------------
# spng
#--------------------------------------------------------------------
add_subdirectory(${SOULCAST_ENGINE_VENDOR_DIR}/libspng)
target_include_directories(SoulcastEngine PRIVATE ${SOULCAST_ENGINE_VENDOR_DIR}/libspng/src)

target_link_libraries(SoulcastEngine PRIVATE spng)


#--------------------------------------------------------------------
# Folder structuring in Visual Studio
#--------------------------------------------------------------------
set_target_properties(SDL3-static PROPERTIES FOLDER "Vendor/Engine")
set_target_properties(SDL_uclibc PROPERTIES FOLDER "Vendor/Engine")
set_target_properties(spng PROPERTIES FOLDER "Vendor/Engine")

if(MSVC_IDE)
	foreach(source IN LISTS SOULCAST_ENGINE_FILES)
		get_filename_component(source_path "${source}" PATH)
		string(REPLACE "${CMAKE_CURRENT_SOURCE_DIR}" "" relative_source_path "${source_path}")
		string(REPLACE "/" "\\" source_path_msvc "${relative_source_path}")
		source_group("${source_path_msvc}" FILES "${source}")
    endforeach()
endif()