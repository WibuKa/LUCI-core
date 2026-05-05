cmake_minimum_required(VERSION 3.10)
project(MyGameEngine C CXX)

# --- Export compile_commands.json (fix LSP) ---
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# --- C++ standard ---
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --- Output ---
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/)

# =========================
# Sources
# =========================
file(GLOB_RECURSE SRC_CPP ${CMAKE_SOURCE_DIR}/src/*.cpp)
file(GLOB_RECURSE SRC_C   ${CMAKE_SOURCE_DIR}/src/*.c)

add_executable(game ${SRC_CPP} ${SRC_C})

# =========================
# Include
# =========================
target_include_directories(game PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/src
)

# =========================
# GLFW
# =========================
find_package(glfw3 REQUIRED)
target_link_libraries(game PRIVATE glfw)

# =========================
# Lua
# =========================
find_package(PkgConfig REQUIRED)
pkg_check_modules(LUA REQUIRED lua5.3)

target_include_directories(game PRIVATE ${LUA_INCLUDE_DIRS})
target_link_libraries(game PRIVATE ${LUA_LIBRARIES})

# =========================
# Platform specific
# =========================
if(WIN32)
    target_link_libraries(game PRIVATE
        opengl32
        gdi32
        user32
        shell32
    )
    target_compile_definitions(game PRIVATE GLFW_STATIC)

elseif(UNIX)
    find_package(OpenGL REQUIRED)
    target_link_libraries(game PRIVATE
        GL
        dl
        pthread
    )
endif()

# =========================
# Build types
# =========================
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    message(STATUS "Release build")
    target_compile_options(game PRIVATE -O2)
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    message(STATUS "Debug build")
    target_compile_options(game PRIVATE -g -Wall)
endif()
