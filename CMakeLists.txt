cmake_minimum_required(VERSION 3.15)
project(VertexEngine)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Build raylib
add_subdirectory(external/raylib)

# Build rlImGui
add_library(rlImGui STATIC
    external/rlImGui/rlImGui.cpp
    external/rlImGui/rlImGuiColors.cpp
)
target_include_directories(rlImGui PUBLIC
    external/rlImGui
    external/imgui
    external/raylib/src
)
target_link_libraries(rlImGui PUBLIC raylib)

# Define executable
add_executable(VertexEngine src/main.cpp)

# Include paths
target_include_directories(VertexEngine PRIVATE
    include
    external/imgui
    external/imgui/backends
    external/ImGuizmo
)

# Link libraries
target_link_libraries(VertexEngine PRIVATE
    rlImGui
    raylib
    winmm
    gdi32
    opengl32
    shell32
)

# Compile options
target_compile_options(VertexEngine PRIVATE -Wall -O3)
