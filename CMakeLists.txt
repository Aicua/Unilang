cmake_minimum_required(VERSION 3.16)
project(UniLang VERSION 1.2.2)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Generate version header
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/src/version.h.in"
    "${CMAKE_CURRENT_BINARY_DIR}/version.h"
    @ONLY
)

# Platform detection
if(WIN32)
    message(STATUS "Building for Windows")
elseif(APPLE)
    message(STATUS "Building for macOS")
elseif(UNIX)
    message(STATUS "Building for Linux/Unix")
endif()

# MSVC settings
if(MSVC)
    add_compile_options(/W4)
    add_compile_options(/utf-8)  # Enable UTF-8 source and execution character sets
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
    add_compile_options(/MT$<$<CONFIG:Debug>:d>)
endif()

# Fetch nlohmann/json library from GitHub
include(FetchContent)
FetchContent_Declare(
    json
    URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
set(JSON_BuildTests OFF CACHE INTERNAL "")
FetchContent_MakeAvailable(json)

# Include directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_BINARY_DIR}  # For generated version.h
)

# Common source files
set(UNILANG_SOURCES
    src/shortcuts_dict.cpp
    src/pattern_matcher.cpp
)

# Common header files
set(UNILANG_HEADERS
    src/shortcuts_dict.h
    src/pattern_matcher.h
    src/platform/platform_types.h
    src/platform/platform_interface.h
)

# Platform-specific sources
if(WIN32)
    list(APPEND UNILANG_SOURCES
        src/main.cpp
        src/keyboard_hook.cpp
        src/text_replacer.cpp
        src/popup_window.cpp
        src/help_window.cpp
        src/update_manager.cpp
        src/auto_updater.cpp
        src/settings_manager.cpp
    )
    list(APPEND UNILANG_HEADERS
        src/keyboard_hook.h
        src/text_replacer.h
        src/popup_window.h
        src/help_window.h
        src/update_manager.h
        src/auto_updater.h
        src/settings_manager.h
        src/resource.h
    )
    set(UNILANG_RESOURCES src/UniLang.rc)
elseif(UNIX AND NOT APPLE)
    # Linux
    list(APPEND UNILANG_SOURCES
        src/platform/linux/main_linux.cpp
        src/platform/linux/keyboard_hook_linux.cpp
        src/platform/linux/text_input_linux.cpp
        src/platform/linux/platform_factory_linux.cpp
    )
    list(APPEND UNILANG_HEADERS
        src/platform/linux/keyboard_hook_linux.h
        src/platform/linux/text_input_linux.h
    )
elseif(APPLE)
    # macOS
    # Enable Objective-C++
    enable_language(OBJCXX)
    set(CMAKE_OBJCXX_STANDARD 17)
    set(CMAKE_OBJCXX_STANDARD_REQUIRED ON)

    list(APPEND UNILANG_SOURCES
        src/platform/macos/main_macos.mm
        src/platform/macos/keyboard_hook_macos.mm
        src/platform/macos/text_input_macos.mm
        src/platform/macos/platform_factory_macos.mm
    )
    list(APPEND UNILANG_HEADERS
        src/platform/macos/keyboard_hook_macos.h
        src/platform/macos/text_input_macos.h
    )
endif()

# Create executable
if(WIN32)
    # Windows GUI app with tray icon
    add_executable(${PROJECT_NAME} WIN32 ${UNILANG_SOURCES} ${UNILANG_HEADERS} ${UNILANG_RESOURCES})
else()
    # Linux/macOS console app for now
    add_executable(${PROJECT_NAME} ${UNILANG_SOURCES} ${UNILANG_HEADERS})
endif()

# Link libraries
target_link_libraries(${PROJECT_NAME} PRIVATE nlohmann_json::nlohmann_json)

if(WIN32)
    # Windows-specific libraries
    target_link_libraries(${PROJECT_NAME} PRIVATE
        user32      # For SendInput, SetWindowsHookEx
        gdi32       # For GDI drawing
        shell32     # For tray icon and ShellExecute
        comctl32    # For common controls
        wininet     # For HTTP requests (update checking)
        urlmon      # For URLDownloadToFile (auto-updater)
        shlwapi     # For path utilities
    )
elseif(UNIX AND NOT APPLE)
    # Linux-specific libraries
    # TODO: Add X11, XTest, or other Linux libraries as needed
    # target_link_libraries(${PROJECT_NAME} PRIVATE X11 Xtst)
elseif(APPLE)
    # macOS-specific frameworks
    find_library(COCOA_FRAMEWORK Cocoa REQUIRED)
    find_library(CARBON_FRAMEWORK Carbon REQUIRED)
    find_library(COREGRAPHICS_FRAMEWORK CoreGraphics REQUIRED)
    find_library(COREFOUNDATION_FRAMEWORK CoreFoundation REQUIRED)

    target_link_libraries(${PROJECT_NAME} PRIVATE
        ${COCOA_FRAMEWORK}
        ${CARBON_FRAMEWORK}
        ${COREGRAPHICS_FRAMEWORK}
        ${COREFOUNDATION_FRAMEWORK}
    )

    # Set bundle properties for macOS app
    set_target_properties(${PROJECT_NAME} PROPERTIES
        MACOSX_BUNDLE FALSE  # Not a bundle app, just a regular executable
    )
endif()

# Windows subsystem settings
if(WIN32)
    if(MSVC)
        set_target_properties(${PROJECT_NAME} PROPERTIES
            LINK_FLAGS "/SUBSYSTEM:WINDOWS"
        )
    else()
        set_target_properties(${PROJECT_NAME} PROPERTIES
            LINK_FLAGS "-mwindows"
        )
    endif()
endif()

# Set output directory
set_target_properties(${PROJECT_NAME} PROPERTIES
    OUTPUT_NAME "UniLang"
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# Note: No external config files needed - shortcuts are embedded in executable

# Install rules
install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION bin
)

message(STATUS "")
message(STATUS "===== UniLang Build Configuration =====")
message(STATUS "Project: ${PROJECT_NAME}")
message(STATUS "Version: ${PROJECT_VERSION}")
message(STATUS "Build Type: ${CMAKE_BUILD_TYPE}")
if(WIN32)
    message(STATUS "Executable: ${CMAKE_BINARY_DIR}/bin/UniLang.exe")
else()
    message(STATUS "Executable: ${CMAKE_BINARY_DIR}/bin/UniLang")
endif()
message(STATUS "========================================")
message(STATUS "")
