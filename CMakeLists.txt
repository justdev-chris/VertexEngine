cmake_minimum_required(VERSION 3.15)
project(VertexEngine)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Just use system-installed raylib or skip rlImGui temporarily
find_library(RAYLIB_LIB raylib)
find_path(RAYLIB_INCLUDE raylib.h)

if(RAYLIB_LIB AND RAYLIB_INCLUDE)
    message(STATUS "Found raylib: ${RAYLIB_LIB}")
else()
    # Download raylib source and build it
    include(FetchContent)
    FetchContent_Declare(raylib
        URL https://github.com/raysan5/raylib/archive/refs/tags/5.0.tar.gz
    )
    FetchContent_MakeAvailable(raylib)
endif()

# Create executable
add_executable(VertexEngine src/main.cpp)

# Include paths
if(RAYLIB_INCLUDE)
    target_include_directories(VertexEngine PRIVATE ${RAYLIB_INCLUDE} include)
else()
    target_include_directories(VertexEngine PRIVATE ${raylib_SOURCE_DIR}/src include)
endif()

# Link libraries
if(RAYLIB_LIB)
    target_link_libraries(VertexEngine PRIVATE ${RAYLIB_LIB})
else()
    target_link_libraries(VertexEngine PRIVATE raylib)
endif()

# Add Windows libraries
target_link_libraries(VertexEngine PRIVATE
    winmm
    gdi32
    opengl32
    shell32
)

# Compile options
target_compile_options(VertexEngine PRIVATE -Wall -O3)
