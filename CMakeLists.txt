cmake_minimum_required(VERSION 3.15)
project(VertexEngine)

set(CMAKE_CXX_STANDARD 17)

# Try to use vcpkg if available
find_package(raylib CONFIG REQUIRED)

# Create executable
add_executable(VertexEngine src/main.cpp)

# raylib package should handle includes automatically
target_include_directories(VertexEngine PRIVATE include)

# Link raylib (vcpkg handles this properly)
target_link_libraries(VertexEngine PRIVATE raylib::raylib)

# Still need Windows libs
if(WIN32)
    target_link_libraries(VertexEngine PRIVATE
        winmm
        gdi32
        opengl32
        shell32
    )
endif()

target_compile_options(VertexEngine PRIVATE -Wall -O3)
