cmake_minimum_required(VERSION 3.15)
project(VertexEngine)

set(CMAKE_CXX_STANDARD 17)

# Skip all the complicated dependency stuff
# Assume raylib is installed system-wide or in a known location

# Find raylib headers and library
find_path(RAYLIB_INCLUDE_DIR raylib.h
    PATHS
    /usr/include
    /usr/local/include
    "C:/raylib/include"
    "$ENV{RAYLIB_PATH}/include"
)

find_library(RAYLIB_LIBRARY
    NAMES raylib
    PATHS
    /usr/lib
    /usr/local/lib
    "C:/raylib/lib"
    "$ENV{RAYLIB_PATH}/lib"
)

if(NOT RAYLIB_INCLUDE_DIR OR NOT RAYLIB_LIBRARY)
    message(FATAL_ERROR "Raylib not found! Please install raylib or set RAYLIB_PATH")
endif()

message(STATUS "Found raylib at: ${RAYLIB_LIBRARY}")

# Create executable
add_executable(VertexEngine src/main.cpp)

# Include directories
target_include_directories(VertexEngine PRIVATE
    include
    ${RAYLIB_INCLUDE_DIR}
)

# Link libraries
target_link_libraries(VertexEngine PRIVATE
    ${RAYLIB_LIBRARY}
    winmm
    gdi32
    opengl32
    shell32
)

# Compile options
target_compile_options(VertexEngine PRIVATE -Wall -O3)
