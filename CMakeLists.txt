cmake_minimum_required(VERSION 3.14)
project(VertexEngine)

include(FetchContent)

# Fetch Raylib (Direct from GitHub)
FetchContent_Declare(raylib
    GIT_REPOSITORY https://github.com/raysan5/raylib.git
    GIT_TAG v5.0
)

# Fetch ImGui
FetchContent_Declare(imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG master
)

FetchContent_MakeAvailable(raylib imgui)

add_executable(VertexEngine src/main.cpp)
target_link_libraries(VertexEngine raylib)
