cmake_minimum_required(VERSION 3.16)
project(VertexEngine CXX)

set(CMAKE_CXX_STANDARD 17)

include(FetchContent)

# 1. Fetch Raylib (Official Repo)
FetchContent_Declare(raylib
    GIT_REPOSITORY https://github.com/raysan5/raylib.git
    GIT_TAG        master
)

# 2. Fetch ImGui
FetchContent_Declare(imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG        master
)

# 3. Fetch ImGuizmo (The 3D Handles)
FetchContent_Declare(imguizmo
    GIT_REPOSITORY https://github.com/CedricGuillemet/ImGuizmo.git
    GIT_TAG        master
)

FetchContent_MakeAvailable(raylib imgui imguizmo)

# Define the Executable
add_executable(VertexEngine src/main.cpp)

# 4. Manually add library Source Files (Crucial Step)
target_sources(VertexEngine PRIVATE
    ${imgui_SOURCE_DIR}/imgui.cpp
    ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/imgui_widgets.cpp
    ${imgui_SOURCE_DIR}/imgui_tables.cpp
    ${imguizmo_SOURCE_DIR}/ImGuizmo.cpp
)

# 5. Include Directories so the compiler finds the .h files
target_include_directories(VertexEngine PRIVATE 
    include 
    ${imgui_SOURCE_DIR}
    ${imgui_SOURCE_DIR}/backends
    ${imguizmo_SOURCE_DIR}
)

# 6. Link Raylib and System Libraries
target_link_libraries(VertexEngine PRIVATE raylib)

# Windows specific flags for a clean build
if(WIN32)
    target_link_libraries(VertexEngine PRIVATE winmm gdi32 opengl32)
endif()
