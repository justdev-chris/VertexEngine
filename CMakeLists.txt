cmake_minimum_required(VERSION 3.15)
project(VertexEngine)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Instead of using external/ directory, download directly with FetchContent
include(FetchContent)

# Download and build raylib
FetchContent_Declare(
    raylib
    URL https://github.com/raysan5/raylib/archive/refs/tags/5.0.tar.gz
)
FetchContent_MakeAvailable(raylib)

# Now we have raylib built, we need to create rlImGui manually
# First, download rlImGui source files directly
set(RLIMGUI_CPP_URL "https://raw.githubusercontent.com/raylib-extras/rlImGui/main/rlImGui.cpp")
set(RLIMGUI_COLORS_URL "https://raw.githubusercontent.com/raylib-extras/rlImGui/main/rlImGuiColors.cpp")
set(RLIMGUI_H_URL "https://raw.githubusercontent.com/raylib-extras/rlImGui/main/rlImGui.h")

# Download the files to build directory
file(DOWNLOAD ${RLIMGUI_CPP_URL} ${CMAKE_BINARY_DIR}/rlImGui.cpp)
file(DOWNLOAD ${RLIMGUI_COLORS_URL} ${CMAKE_BINARY_DIR}/rlImGuiColors.cpp)
file(DOWNLOAD ${RLIMGUI_H_URL} ${CMAKE_BINARY_DIR}/rlImGui.h)

# Create rlImGui library
add_library(rlImGui STATIC
    ${CMAKE_BINARY_DIR}/rlImGui.cpp
    ${CMAKE_BINARY_DIR}/rlImGuiColors.cpp
)

target_include_directories(rlImGui PUBLIC
    ${CMAKE_BINARY_DIR}  # For rlImGui.h
    ${raylib_SOURCE_DIR}/src  # For raylib headers
)

target_link_libraries(rlImGui PUBLIC raylib)

# Download ImGui (header-only)
set(IMGUI_URL "https://github.com/ocornut/imgui/archive/refs/tags/v1.90.7.tar.gz")
FetchContent_Declare(imgui URL ${IMGUI_URL})
FetchContent_MakeAvailable(imgui)

# Download ImGuizmo header
set(IMGUIZMO_URL "https://raw.githubusercontent.com/CedricGuillemet/ImGuizmo/master/ImGuizmo.h")
file(DOWNLOAD ${IMGUIZMO_URL} ${CMAKE_BINARY_DIR}/ImGuizmo.h)

# Define executable
add_executable(VertexEngine src/main.cpp)

# Include directories
target_include_directories(VertexEngine PRIVATE
    include
    ${imgui_SOURCE_DIR}           # ImGui headers
    ${imgui_SOURCE_DIR}/backends  # ImGui backends
    ${CMAKE_BINARY_DIR}           # rlImGui.h and ImGuizmo.h
    ${raylib_SOURCE_DIR}/src      # raylib headers
)

# Link libraries
target_link_libraries(VertexEngine PRIVATE
    rlImGui  # Our custom library
    raylib   # From FetchContent
    winmm
    gdi32
    opengl32
    shell32
)

# Compile options
target_compile_options(VertexEngine PRIVATE -Wall -O3)
