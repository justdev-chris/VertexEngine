cmake_minimum_required(VERSION 3.15)
project(VertexEngine)

set(CMAKE_CXX_STANDARD 17)

# Download raylib
if(NOT EXISTS "${CMAKE_SOURCE_DIR}/raylib-5.0_win64_mingw-w64")
    message(STATUS "Downloading raylib...")
    file(DOWNLOAD 
        "https://github.com/raysan5/raylib/releases/download/5.0/raylib-5.0_win64_mingw-w64.zip"
        "${CMAKE_BINARY_DIR}/raylib.zip"
    )
    execute_process(
        COMMAND ${CMAKE_COMMAND} -E tar xzf "${CMAKE_BINARY_DIR}/raylib.zip"
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    )
endif()

set(RAYLIB_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/raylib-5.0_win64_mingw-w64/include")
set(RAYLIB_LIBRARY "${CMAKE_SOURCE_DIR}/raylib-5.0_win64_mingw-w64/lib/libraylib.a")

# Download tinygltf
if(NOT EXISTS "${CMAKE_SOURCE_DIR}/tiny_gltf.h")
    message(STATUS "Downloading tiny_gltf.h...")
    file(DOWNLOAD
        "https://raw.githubusercontent.com/syoyo/tinygltf/master/tiny_gltf.h"
        "${CMAKE_SOURCE_DIR}/tiny_gltf.h"
    )
endif()

if(NOT EXISTS "${CMAKE_SOURCE_DIR}/json.hpp")
    message(STATUS "Downloading json.hpp...")
    file(DOWNLOAD
        "https://raw.githubusercontent.com/nlohmann/json/develop/single_include/nlohmann/json.hpp"
        "${CMAKE_SOURCE_DIR}/json.hpp"
    )
endif()

if(NOT EXISTS "${CMAKE_SOURCE_DIR}/stb_image.h")
    message(STATUS "Downloading stb_image.h...")
    file(DOWNLOAD
        "https://raw.githubusercontent.com/nothings/stb/master/stb_image.h"
        "${CMAKE_SOURCE_DIR}/stb_image.h"
    )
endif()

# Download ImGui if needed
if(NOT EXISTS "${CMAKE_SOURCE_DIR}/imgui.h")
    message(STATUS "Downloading ImGui...")
    file(DOWNLOAD
        "https://raw.githubusercontent.com/ocornut/imgui/master/imgui.h"
        "${CMAKE_SOURCE_DIR}/imgui.h"
    )
    file(DOWNLOAD
        "https://raw.githubusercontent.com/ocornut/imgui/master/imconfig.h"
        "${CMAKE_SOURCE_DIR}/imconfig.h"
    )
    file(DOWNLOAD
        "https://raw.githubusercontent.com/ocornut/imgui/master/imgui.cpp"
        "${CMAKE_SOURCE_DIR}/imgui.cpp"
    )
endif()

# Download ImGuizmo if needed
if(NOT EXISTS "${CMAKE_SOURCE_DIR}/ImGuizmo.h")
    message(STATUS "Downloading ImGuizmo...")
    file(DOWNLOAD
        "https://raw.githubusercontent.com/CedricGuillemet/ImGuizmo/master/ImGuizmo.h"
        "${CMAKE_SOURCE_DIR}/ImGuizmo.h"
    )
endif()

# Download rlImGui if needed
if(NOT EXISTS "${CMAKE_SOURCE_DIR}/rlImGui.h")
    message(STATUS "Downloading rlImGui...")
    file(DOWNLOAD
        "https://raw.githubusercontent.com/raylib-extras/rlImGui/main/rlImGui.h"
        "${CMAKE_SOURCE_DIR}/rlImGui.h"
    )
    file(DOWNLOAD
        "https://raw.githubusercontent.com/raylib-extras/rlImGui/main/rlImGui.cpp"
        "${CMAKE_SOURCE_DIR}/rlImGui.cpp"
    )
    file(DOWNLOAD
        "https://raw.githubusercontent.com/raylib-extras/rlImGui/main/rlImGuiColors.cpp"
        "${CMAKE_SOURCE_DIR}/rlImGuiColors.cpp"
    )
endif()

# Create executable
add_executable(VertexEngine 
    src/main.cpp
    # Add ImGui sources if they exist
    $<$<EXISTS:${CMAKE_SOURCE_DIR}/imgui.cpp>:${CMAKE_SOURCE_DIR}/imgui.cpp>
    $<$<EXISTS:${CMAKE_SOURCE_DIR}/rlImGui.cpp>:${CMAKE_SOURCE_DIR}/rlImGui.cpp>
    $<$<EXISTS:${CMAKE_SOURCE_DIR}/rlImGuiColors.cpp>:${CMAKE_SOURCE_DIR}/rlImGuiColors.cpp>
)

# Include directories
target_include_directories(VertexEngine PRIVATE
    include
    ${RAYLIB_INCLUDE_DIR}
    ${CMAKE_SOURCE_DIR}  # For all downloaded headers in project root
)

# Link libraries
target_link_libraries(VertexEngine PRIVATE
    ${RAYLIB_LIBRARY}
    winmm
    gdi32
    opengl32
    shell32
)

# Static linking
set_target_properties(VertexEngine PROPERTIES
    LINK_FLAGS "-static -static-libgcc -static-libstdc++"
)

target_compile_options(VertexEngine PRIVATE -Wall -O3)
