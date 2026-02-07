cmake_minimum_required(VERSION 3.15...4.0)

project(QuadtreeAstar LANGUAGES C CXX)


# ------------------ Compile with GUI ------------------ #
find_package(raylib REQUIRED)
find_package(unordered_dense REQUIRED)
add_executable(QuadtreeAstar)

target_include_directories(QuadtreeAstar PUBLIC include)

set(CMAKE_COMPILE_WARNING_AS_ERROR ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

target_link_libraries(QuadtreeAstar raylib unordered_dense::unordered_dense)
target_compile_options(QuadtreeAstar PRIVATE -O3)


target_sources(QuadtreeAstar PRIVATE
    source/algorithm/quadtree/Quadtree.cpp
    source/rendering/Renderer.cpp
    source/Main.cpp
)