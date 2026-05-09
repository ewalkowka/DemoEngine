cmake_minimum_required(VERSION 3.20)

project(DemoEngine)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

set(SDL_SHARED OFF CACHE BOOL "" FORCE)
set(SDL_STATIC ON CACHE BOOL "" FORCE)

FetchContent_Declare(
    SDL3
    GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
    GIT_TAG release-3.2.10
)

FetchContent_MakeAvailable(SDL3)

add_executable(DemoEngine
    src/main.cpp
    external/glad/src/gl.c
)

target_include_directories(DemoEngine PRIVATE
    external/glad/include
)

target_link_libraries(DemoEngine PRIVATE
    SDL3::SDL3-static
)