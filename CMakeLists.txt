cmake_minimum_required(VERSION 3.10)
project(MyGameEngine LANGUAGES C CXX)

# --- C++17 ---
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --- Default build type ---
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

# --- Export compile_commands.json ---
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# --- Output folder ---
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/linux)

# --- Source files ---
file(GLOB_RECURSE SRC_CPP src/*.cpp)
file(GLOB_RECURSE SRC_C   src/*.c)

add_executable(game
    ${SRC_CPP}
    ${SRC_C}
)

# =====================================================
# INCLUDE DIRECTORIES
# =====================================================
target_include_directories(game PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/src
)

# =====================================================
# LUA STATIC LIB (THEO PLATFORM)
# =====================================================
if (WIN32)
    message(STATUS "Linking Lua static (Windows)")

    target_link_libraries(game PRIVATE
        ${CMAKE_SOURCE_DIR}/windows/lib
    )
else()
    message(STATUS "Linux build")

    find_package(OpenGL REQUIRED)
    find_package(Threads REQUIRED)

    target_link_libraries(game PRIVATE
        # Lua
        ${CMAKE_SOURCE_DIR}/linux/lib/liblua.a

        # GLFW
        ${CMAKE_SOURCE_DIR}/linux/lib/libglfw3.a
        
        OpenGL::GL
        Threads::Threads
        dl
        X11
        Xrandr
        Xi
        Xinerama
        Xcursor
        m
    )
endif()

# --- Warnings ---
if(UNIX AND NOT APPLE)
    target_compile_options(game PRIVATE -Wall)
endif()

# --- Debug ---
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_options(game PRIVATE -g)
endif()

# --- Release ---
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    target_compile_options(game PRIVATE -O2)
endif()

