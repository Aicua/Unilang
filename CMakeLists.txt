cmake_minimum_required(VERSION 3.16)
project(UniLang VERSION 1.2.1)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Generate version header
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/src/version.h.in"
    "${CMAKE_CURRENT_BINARY_DIR}/version.h"
    @ONLY
)

# Windows-only for now
if(NOT WIN32)
    message(FATAL_ERROR "UniLang currently only supports Windows")
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

# Source files
set(UNILANG_SOURCES
    src/main.cpp
    src/keyboard_hook.cpp
    src/shortcuts_dict.cpp
    src/text_replacer.cpp
    src/pattern_matcher.cpp
    src/popup_window.cpp
    src/settings_manager.cpp
    src/help_window.cpp
    src/update_manager.cpp
    src/auto_updater.cpp
)

# Header files
set(UNILANG_HEADERS
    src/keyboard_hook.h
    src/shortcuts_dict.h
    src/text_replacer.h
    src/pattern_matcher.h
    src/popup_window.h
    src/settings_manager.h
    src/help_window.h
    src/update_manager.h
    src/auto_updater.h
    src/resource.h
)

# Resource files
set(UNILANG_RESOURCES
    src/UniLang.rc
)

# Create executable (Windows GUI app with tray icon)
add_executable(${PROJECT_NAME} WIN32 ${UNILANG_SOURCES} ${UNILANG_HEADERS} ${UNILANG_RESOURCES})

# Link libraries
target_link_libraries(${PROJECT_NAME} PRIVATE
    nlohmann_json::nlohmann_json
    user32      # For SendInput, SetWindowsHookEx
    gdi32       # For GDI drawing
    shell32     # For tray icon and ShellExecute
    comctl32    # For common controls
    wininet     # For HTTP requests (update checking)
    urlmon      # For URLDownloadToFile (auto-updater)
    shlwapi     # For path utilities
)

# Windows subsystem settings
if(MSVC)
    set_target_properties(${PROJECT_NAME} PROPERTIES
        LINK_FLAGS "/SUBSYSTEM:WINDOWS"
    )
else()
    set_target_properties(${PROJECT_NAME} PROPERTIES
        LINK_FLAGS "-mwindows"
    )
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
message(STATUS "Executable: ${CMAKE_BINARY_DIR}/bin/UniLang.exe")
message(STATUS "========================================")
message(STATUS "")
