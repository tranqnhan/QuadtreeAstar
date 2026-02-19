cmake_minimum_required(VERSION 3.15...4.0)

project(QuadtreeAstar LANGUAGES C CXX)

# cmake -B build -S . -DRAYLIB_PATH=$HOME/libs/raylib
# cmake --build build

# emcmake cmake -B build-web -S . -DRAYLIB_PATH=$HOME/libs/raylib
# cmake --build build-web

# ------------------ Compile with GUI ------------------ #

add_executable(QuadtreeAstar)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build" FORCE)
endif()

# User provides path to raylib
if(NOT RAYLIB_PATH)
    message(FATAL_ERROR "Please set -DRAYLIB_PATH=/path/to/raylib")
endif()

add_subdirectory(${RAYLIB_PATH} raylib-build)


target_include_directories(QuadtreeAstar PUBLIC include library/include) 

set(CMAKE_COMPILE_WARNING_AS_ERROR ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

target_link_libraries(QuadtreeAstar raylib)
target_compile_options(QuadtreeAstar PRIVATE -O2)

target_sources(QuadtreeAstar PRIVATE
    source/BinaryMath.cpp
    source/algorithm/quadtree/Quadtree.cpp
    source/algorithm/quadtree/QuadtreeRenderer.cpp
    source/grid/ImageGridEnvironment.cpp
    source/drawpad/Drawpad.cpp
    source/Main.cpp
)


if(EMSCRIPTEN)
    set(CMAKE_EXECUTABLE_SUFFIX ".js")

    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DPLATFORM_WEB")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DPLATFORM_WEB")

    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -s ASYNCIFY -s MODULARIZE=1 -s EXPORT_ES6=1")
endif()