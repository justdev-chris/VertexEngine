cmake_minimum_required(VERSION 3.12)
project(VertexEngine)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Download dependencies
include(FetchContent)

FetchContent_Declare(raylib URL https://github.com/raysan5/raylib/archive/master.tar.gz)
FetchContent_Declare(imgui URL https://github.com/ocornut/imgui/archive/master.tar.gz)
FetchContent_Declare(rlimgui URL https://github.com/raylib-extras/rlImGui/archive/main.tar.gz)
FetchContent_Declare(imguizmo URL https://github.com/CedricGuillemet/ImGuizmo/archive/master.tar.gz)
FetchContent_Declare(tinygltf URL https://github.com/syoyo/tinygltf/archive/master.tar.gz)

FetchContent_MakeAvailable(raylib imgui rlimgui imguizmo tinygltf)

# Define our executable
add_executable(VertexEngine src/main.cpp)

# Header include paths
target_include_directories(VertexEngine PRIVATE 
    include
    ${imgui_SOURCE_DIR}
    ${imgui_SOURCE_DIR}/backends
    ${imguizmo_SOURCE_DIR}
    ${tinygltf_SOURCE_DIR}
    ${raylib_SOURCE_DIR}/src
    ${rlimgui_SOURCE_DIR}
)

# Link everything together
# Order matters: Raylib should be near the end to provide symbols for wrappers
target_link_libraries(VertexEngine PRIVATE 
    rlImGui
    imguizmo
    raylib
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
