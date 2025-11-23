cmake_minimum_required(VERSION 3.28.1)

project(SimulationGame
    VERSION 0.1
    DESCRIPTION "SimulationGame"
    LANGUAGES CXX)

set (CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set (CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set (CMAKE_EXPORT_COMPILE_COMMANDS On)

set(CMAKE_ALL_ERRORS_FATAL TRUE)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include (cmake/CPM.cmake)

set(CPM_SOURCE_CACHE "./cpm-cache")

CPMAddPackage(
    URI "gh:libsdl-org/SDL#release-3.2.26"
)
CPMAddPackage(
    URI "gh:libsdl-org/SDL_ttf#release-3.2.2"
)
CPMAddPackage(
    URI "gh:KhronosGroup/SPIRV-Headers#vulkan-sdk-1.4.328.1"
)
set(SPIRV_WERROR OFF)
CPMAddPackage(
    URI "gh:KhronosGroup/SPIRV-Tools#vulkan-sdk-1.4.328.1"
)
CPMAddPackage(
    URI "gh:KhronosGroup/glslang#vulkan-sdk-1.4.328.1"
)
CPMAddPackage(
    URI "gh:KhronosGroup/SPIRV-Cross#vulkan-sdk-1.4.328.1"
)

CPMAddPackage(
    URI "gh:g-truc/glm#1.0.2")

set(CMAKE_COMPILE_WARNING_AS_ERROR ON)

add_executable(${PROJECT_NAME}
    ./src/main.cpp
    ./src/SimulationGame.cpp
    ./src/core/AssetLoader.cpp
    ./src/core/Engine.cpp
    ./src/core/Environment.cpp
    ./src/core/Filesystem.cpp
    ./src/core/Logger.cpp
    ./src/graphics/Renderer.cpp
    ./src/graphics/ShaderCross.cpp
    ./src/graphics/TextRenderer.cpp
    ./src/sdl/SDL.cpp
    ./src/sdl/TTF.cpp
)

target_include_directories(${PROJECT_NAME} PRIVATE ./src)

target_link_libraries(${PROJECT_NAME} PRIVATE
    SDL3::SDL3
    SDL3_ttf::SDL3_ttf
    SPIRV
    glslang
    glslang-default-resource-limits
    glm)
