cmake_minimum_required(VERSION 3.15)
project(VertexEngine)

set(CMAKE_CXX_STANDARD 17)

# Assume raylib is in the expected location
set(RAYLIB_DIR "${CMAKE_SOURCE_DIR}/raylib-5.0_win64_mingw-w64")
set(RAYLIB_INCLUDE_DIR "${RAYLIB_DIR}/include")
set(RAYLIB_LIBRARY "${RAYLIB_DIR}/lib/libraylib.a")

# Check if exists, if not, warn but continue
if(NOT EXISTS ${RAYLIB_LIBRARY})
    message(WARNING "Raylib static library not found at ${RAYLIB_LIBRARY}")
    message(WARNING "Make sure to download raylib-5.0_win64_mingw-w64.zip")
    message(WARNING "and extract it to the project root directory")
    
    # Fallback: try system installation
    find_library(RAYLIB_LIB NAMES raylib)
    if(RAYLIB_LIB)
        set(RAYLIB_LIBRARY ${RAYLIB_LIB})
    else()
        set(RAYLIB_LIBRARY raylib)  # Hope linker finds it
    endif()
endif()

# Create executable
add_executable(VertexEngine src/main.cpp)

# Include directories
if(EXISTS ${RAYLIB_INCLUDE_DIR})
    target_include_directories(VertexEngine PRIVATE ${RAYLIB_INCLUDE_DIR})
endif()

target_include_directories(VertexEngine PRIVATE include)

# Link libraries - STATIC linking
target_link_libraries(VertexEngine PRIVATE
    ${RAYLIB_LIBRARY}
    winmm
    gdi32
    opengl32
    shell32
)

# STATIC linking flags to avoid DLL dependencies
set_target_properties(VertexEngine PROPERTIES
    LINK_FLAGS "-static -static-libgcc -static-libstdc++"
)

target_compile_options(VertexEngine PRIVATE -Wall -O3)
