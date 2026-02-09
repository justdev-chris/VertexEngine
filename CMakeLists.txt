cmake_minimum_required(VERSION 3.16)
project(VertexEngine CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Force Release mode for GitHub artifacts to ensure performance on the Mini PC
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

include(FetchContent)

# 1. Fetch Raylib
FetchContent_Declare(raylib
    GIT_REPOSITORY https://github.com/raysan5/raylib.git
    GIT_TAG        master
)

# 2. Fetch ImGui (Core logic)
FetchContent_Declare(imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG        master
)

# 3. Fetch ImGuizmo (The 3D Manipulator)
FetchContent_Declare(imguizmo
    GIT_REPOSITORY https://github.com/CedricGuillemet/ImGuizmo.git
    GIT_TAG        master
)

FetchContent_MakeAvailable(raylib imgui imguizmo)

# Define the executable
add_executable(VertexEngine src/main.cpp)

# 4. Universal Source Inclusion
# We must compile the .cpp files from the fetched dependencies 
# because they are not header-only libraries.
target_sources(VertexEngine PRIVATE
    ${imgui_SOURCE_DIR}/imgui.cpp
    ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/imgui_widgets.cpp
    ${imgui_SOURCE_DIR}/imgui_tables.cpp
    ${imgui_SOURCE_DIR}/imgui_demo.cpp
    ${imguizmo_SOURCE_DIR}/ImGuizmo.cpp
)

# 5. Universal Include Paths
# This tells the compiler where to look for #include "ImGuizmo.h" etc.
target_include_directories(VertexEngine PRIVATE 
    include 
    ${imgui_SOURCE_DIR}
    ${imgui_SOURCE_DIR}/backends
    ${imguizmo_SOURCE_DIR}
)

# 6. Linking
# raylib is a CMake project, so it handles its own internal linking.
target_link_libraries(VertexEngine PRIVATE raylib)

# Windows-specific system libraries for the Mini PC build
if(WIN32)
    target_link_libraries(VertexEngine PRIVATE winmm gdi32 opengl32 shell32)
    # Hide the console window in Release mode if desired
    # set_target_properties(VertexEngine PROPERTIES WIN32_EXECUTABLE TRUE)
endif()
