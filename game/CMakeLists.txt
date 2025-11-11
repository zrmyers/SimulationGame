cmake_minimum_required(VERSION 3.22)

add_executable(${PROJECT_NAME}
    ./src/main.cpp
    ./src/core/Engine.cpp
    ./src/core/Logger.cpp
    ./src/sdl/SDL.cpp
)

target_include_directories(${PROJECT_NAME} PRIVATE ./src)

target_link_libraries(${PROJECT_NAME} PRIVATE SDL3::SDL3)