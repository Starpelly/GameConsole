cmake_minimum_required(VERSION 3.15)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(SoulcastPlayer)

set(SOULCAST_PLAYER_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(SOULCAST_PLAYER_VENDOR_DIR ${CMAKE_CURRENT_SOURCE_DIR}/vendor)

file(GLOB_RECURSE SOULCAST_PLAYER_FILES
    ${SOULCAST_PLAYER_SOURCE_DIR}/*.cpp
    ${SOULCAST_PLAYER_SOURCE_DIR}/*.hpp
)

add_executable(SoulcastPlayer ${SOULCAST_PLAYER_FILES})
target_link_libraries(SoulcastPlayer PRIVATE SoulcastEngine)
target_include_directories(SoulcastPlayer PRIVATE ${SOULCAST_PLAYER_SOURCE_DIR})
target_include_directories(SoulcastPlayer PRIVATE ${CMAKE_SOURCE_DIR}/Engine/src)
target_include_directories(SoulcastPlayer PRIVATE ${CMAKE_SOURCE_DIR}/Engine/vendor/SDL3/include)

#--------------------------------------------------------------------
# DLL copy post-build step (Windows only)
#--------------------------------------------------------------------
if(WIN32)
    add_custom_command(TARGET SoulcastPlayer POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:SoulcastEngine>
            $<TARGET_FILE_DIR:SoulcastPlayer>
    )
endif()

#--------------------------------------------------------------------
# Folder structuring in Visual Studio
#--------------------------------------------------------------------
if(MSVC_IDE)
	set_target_properties(SDL3-static PROPERTIES FOLDER "Vendor/SDL3")
	set_target_properties(SDL_uclibc PROPERTIES FOLDER "Vendor/SDL3")

	foreach(source IN LISTS SOULCAST_PLAYER_FILES)
		get_filename_component(source_path "${source}" PATH)
		string(REPLACE "${CMAKE_CURRENT_SOURCE_DIR}" "" relative_source_path "${source_path}")
		string(REPLACE "/" "\\" source_path_msvc "${relative_source_path}")
		source_group("${source_path_msvc}" FILES "${source}")
    endforeach()
endif()