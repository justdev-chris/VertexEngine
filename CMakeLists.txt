cmake_minimum_required(VERSION 3.12)
project(VertexEngine)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Download dependencies
include(FetchContent)

FetchContent_Declare(
    raylib
    URL https://github.com/raysan5/raylib/archive/refs/tags/5.0.tar.gz  # Use a stable version
)

FetchContent_Declare(
    imgui
    URL https://github.com/ocornut/imgui/archive/refs/tags/v1.90.7.tar.gz
)

FetchContent_Declare(
    rlimgui
    URL https://github.com/raylib-extras/rlImGui/archive/refs/tags/v4.2.tar.gz
)

FetchContent_Declare(
    tinygltf
    URL https://github.com/syoyo/tinygltf/archive/refs/tags/v2.8.20.tar.gz
)

# Fetch but don't make available yet
FetchContent_MakeAvailable(raylib imgui)

# rlImGui depends on both raylib and imgui, so we need to handle it specially
FetchContent_GetProperties(rlimgui)
if(NOT rlimgui_POPULATED)
    FetchContent_Populate(rlimgui)
    add_library(rlImGui STATIC 
        ${rlimgui_SOURCE_DIR}/rlImGui.cpp
        ${rlimgui_SOURCE_DIR}/rlImGuiColors.cpp
    )
    target_include_directories(rlImGui PUBLIC 
        ${rlimgui_SOURCE_DIR}
        ${imgui_SOURCE_DIR}
        ${raylib_SOURCE_DIR}/src
    )
    target_link_libraries(rlImGui PUBLIC raylib)
endif()

# ImGuizmo is header-only, so we need to handle it specially
FetchContent_GetProperties(imguizmo)
if(NOT imguizmo_POPULATED)
    FetchContent_Populate(imguizmo)
    add_library(imguizmo INTERFACE)
    target_include_directories(imguizmo INTERFACE 
        ${imguizmo_SOURCE_DIR}
        ${imgui_SOURCE_DIR}
    )
endif()

# tinygltf is also header-only
FetchContent_GetProperties(tinygltf)
if(NOT tinygltf_POPULATED)
    FetchContent_Populate(tinygltf)
    add_library(tinygltf INTERFACE)
    target_include_directories(tinygltf INTERFACE ${tinygltf_SOURCE_DIR})
endif()

# Define our executable
add_executable(VertexEngine src/main.cpp)

# Header include paths
target_include_directories(VertexEngine PRIVATE 
    include
    ${imgui_SOURCE_DIR}
    ${imgui_SOURCE_DIR}/backends
)

# Link everything together
target_link_libraries(VertexEngine PRIVATE 
    rlImGui          # Our static library
    imguizmo         # Header-only library
    raylib           # From FetchContent
    tinygltf         # Header-only library
    
    # Windows libraries
    winmm
    gdi32
    opengl32
    shell32
)

# Build configurations for Windows
if(MSVC)
    target_compile_options(VertexEngine PRIVATE /W4)
else()
    target_compile_options(VertexEngine PRIVATE -Wall -O3)
endif()
