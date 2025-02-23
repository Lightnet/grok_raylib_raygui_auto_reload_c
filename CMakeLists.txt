# Minimum CMake version required to build the project
cmake_minimum_required(VERSION 3.10)

# Project name and language (C)
project(RaylibExample C)

# Set C standard to C11 for modern features and ensure it’s enforced
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Define output directories for all build artifacts (executables, libraries, archives)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}")

# Specific output directories for Debug configuration
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG "${PROJECT_BINARY_DIR}/Debug")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG "${PROJECT_BINARY_DIR}/Debug")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG "${PROJECT_BINARY_DIR}/Debug")

# Force static linking for fetched libraries (raylib, raygui)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "Default to static libs" FORCE)

# Include FetchContent module to download external dependencies
include(FetchContent)

# Fetch Raylib 5.5 from GitHub
FetchContent_Declare(
    raylib
    GIT_REPOSITORY https://github.com/raysan5/raylib.git
    GIT_TAG 5.5  # Latest stable release as of Nov 18, 2024
)
FetchContent_MakeAvailable(raylib)

# Fetch RayGUI 4.0 from GitHub
FetchContent_Declare(
    raygui
    GIT_REPOSITORY https://github.com/raysan5/raygui.git
    GIT_TAG 4.0
)
FetchContent_MakeAvailable(raygui)

# Define the main executable target (RaylibExample.exe)
add_executable(${PROJECT_NAME} src/main.c)
target_include_directories(${PROJECT_NAME} PUBLIC 
    ${CMAKE_SOURCE_DIR}/include  # Custom header directory
    ${raygui_SOURCE_DIR}/src    # RayGUI header directory
)

# Define the shared library target (game.dll)
add_library(game SHARED src/game.c)
target_include_directories(game PUBLIC 
    ${CMAKE_SOURCE_DIR}/include
    ${raygui_SOURCE_DIR}/src
)
set_target_properties(game PROPERTIES 
    OUTPUT_NAME "game"  # Name the DLL 'game.dll'
)

# Link both targets to Raylib static library
target_link_libraries(${PROJECT_NAME} raylib)
target_link_libraries(game raylib)

# Windows-specific linking for multimedia and graphics libraries
if (WIN32)
    target_link_libraries(${PROJECT_NAME} winmm.lib gdi32.lib)
    target_link_libraries(game winmm.lib gdi32.lib)
endif()

# MSVC-specific compiler flags to suppress warnings
if (MSVC)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS /wd4267)  # Suppresses C4267 (size_t to int)
endif()