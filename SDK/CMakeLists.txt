cmake_minimum_required(VERSION 3.16)

project(SoulcastSDK VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets WebEngineWidgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets WebEngineWidgets)

set(SOULCAST_SDK_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(SOULCAST_SDK_VENDOR_DIR ${CMAKE_CURRENT_SOURCE_DIR}/vendor)
set(SOULCAST_SDK_RESOURCES_DIR ${CMAKE_CURRENT_SOURCE_DIR}/resources)

set(PROJECT_SOURCES
    ${SOULCAST_SDK_SOURCE_DIR}/main.cpp
    ${SOULCAST_SDK_SOURCE_DIR}/MainWindow.cpp
    ${SOULCAST_SDK_SOURCE_DIR}/MainWindow.hpp
    ${SOULCAST_SDK_SOURCE_DIR}/mainwindow.ui
    ${SOULCAST_SDK_SOURCE_DIR}/AssetBrowser.cpp
    ${SOULCAST_SDK_SOURCE_DIR}/AssetBrowser.hpp
    ${SOULCAST_SDK_SOURCE_DIR}/CodeEditor.hpp
    ${SOULCAST_SDK_SOURCE_DIR}/CodeEditor.cpp
    ${SOULCAST_SDK_SOURCE_DIR}/codeeditor.ui
    ${SOULCAST_SDK_SOURCE_DIR}/SDK.hpp
    ${SOULCAST_SDK_SOURCE_DIR}/SDK.cpp
    ${SOULCAST_SDK_SOURCE_DIR}/GameWindow.hpp
    ${SOULCAST_SDK_SOURCE_DIR}/GameWindow.cpp
    ${SOULCAST_SDK_SOURCE_DIR}/gamewindow.ui
    ${SOULCAST_SDK_SOURCE_DIR}/SDLRenderer.hpp
    ${SOULCAST_SDK_SOURCE_DIR}/SDLRenderer.cpp
    ${SOULCAST_SDK_SOURCE_DIR}/SDLKeymap.cpp
    ${SOULCAST_SDK_SOURCE_DIR}/SDLKeymap.hpp
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(SoulcastSDK
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET SoulcastSDK APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(SoulcastSDK SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(SoulcastSDK
            ${PROJECT_SOURCES}
            ${SOULCAST_SDK_RESOURCES_DIR}/main.qrc
        )
    endif()
endif()

target_link_libraries(SoulcastSDK PRIVATE Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::WebEngineWidgets)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.SoulcastSDK)
endif()
set_target_properties(SoulcastSDK PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS SoulcastSDK
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(SoulcastSDK)
endif()

#--------------------------------------------------------------------
# Engine DLL copy post-build step (Windows only)
#--------------------------------------------------------------------
target_link_libraries(SoulcastSDK PRIVATE SoulcastEngine)
target_include_directories(SoulcastSDK PRIVATE ${SOULCAST_SDK_SOURCE_DIR})
target_include_directories(SoulcastSDK PRIVATE ${CMAKE_SOURCE_DIR}/Engine/src)
target_include_directories(SoulcastSDK PRIVATE ${CMAKE_SOURCE_DIR}/Engine/vendor/SDL3/include)

if(WIN32)
    add_custom_command(TARGET SoulcastSDK POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:SoulcastEngine>
            $<TARGET_FILE_DIR:SoulcastSDK>
    )
endif()

#--------------------------------------------------------------------
# SDL3
#--------------------------------------------------------------------
set(SDL_TEST_LIBRARY OFF)
set(SDL_STATIC ON)
set(SDL_SHARED OFF)

target_include_directories(SoulcastSDK PRIVATE ${CMAKE_SOURCE_DIR}/Engine/vendor/SDL3/include)
target_link_libraries(SoulcastSDK PRIVATE SDL3-static)

#--------------------------------------------------------------------
# External libraries
#--------------------------------------------------------------------
add_subdirectory(${SOULCAST_SDK_VENDOR_DIR}/phantom)
target_link_libraries(SoulcastSDK PRIVATE phantom)

add_subdirectory(${SOULCAST_SDK_VENDOR_DIR}/QtPropertyBrowser)
target_link_libraries(SoulcastSDK PRIVATE QtPropertyBrowser)

add_subdirectory(${SOULCAST_SDK_VENDOR_DIR}/QCodeEditor)
target_link_libraries(SoulcastSDK PRIVATE QCodeEditor)
target_include_directories(SoulcastSDK PRIVATE ${SOULCAST_SDK_VENDOR_DIR}/QCodeEditor/include)

#--------------------------------------------------------------------
# Folder structuring in Visual Studio
#--------------------------------------------------------------------
set_target_properties(phantom PROPERTIES FOLDER "Vendor/SDK")
set_target_properties(QCodeEditor PROPERTIES FOLDER "Vendor/SDK")
set_target_properties(QtPropertyBrowser PROPERTIES FOLDER "Vendor/SDK")

if(MSVC_IDE)
    source_group("" FILES ${PROJECT_SOURCES})

    foreach(source IN LISTS PROJECT_SOURCES)
        get_filename_component(source_path "${source}" PATH)
        string(REPLACE "${CMAKE_CURRENT_SOURCE_DIR}" "" relative_source_path "${source_path}")
        string(REPLACE "/" "\\" source_path_msvc "${relative_source_path}")
        source_group("${source_path_msvc}" FILES "${source}")
    endforeach()
endif()
