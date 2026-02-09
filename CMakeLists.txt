cmake_minimum_required(VERSION 3.16)
project(VertexEngine CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

include(FetchContent)

# 1. Fetch Raylib
FetchContent_Declare(raylib
    GIT_REPOSITORY https://github.com/raysan5/raylib.git
    GIT_TAG        master
)

# 2. Fetch ImGui
FetchContent_Declare(imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG        master
)

# 3. Fetch ImGuizmo
FetchContent_Declare(imguizmo
    GIT_REPOSITORY https://github.com/CedricGuillemet/ImGuizmo.git
    GIT_TAG        master
)

# 4. Fetch TinyGLTF (Corrected branch name)
FetchContent_Declare(tinygltf
    GIT_REPOSITORY https://github.com/syoyo/tinygltf.git
    GIT_TAG        release
)

FetchContent_MakeAvailable(raylib imgui imguizmo tinygltf)

add_executable(VertexEngine src/main.cpp)

# Define source files for dependencies that aren't header-only
target_sources(VertexEngine PRIVATE
    ${imgui_SOURCE_DIR}/imgui.cpp
    ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/imgui_widgets.cpp
    ${imgui_SOURCE_DIR}/imgui_tables.cpp
    ${imgui_SOURCE_DIR}/imgui_demo.cpp
    ${imguizmo_SOURCE_DIR}/ImGuizmo.cpp
)

# Universal Include Paths
target_include_directories(VertexEngine PRIVATE 
    include 
    ${imgui_SOURCE_DIR}
    ${imgui_SOURCE_DIR}/backends
    ${imguizmo_SOURCE_DIR}
    ${tinygltf_SOURCE_DIR}
    ${raylib_SOURCE_DIR}/src
)

# Link Raylib (handles its own dependencies)
target_link_libraries(VertexEngine PRIVATE raylib)

# Windows System Libraries
if(WIN32)
    target_link_libraries(VertexEngine PRIVATE winmm gdi32 opengl32 shell32)
endif()
