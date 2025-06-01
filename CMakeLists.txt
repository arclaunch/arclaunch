cmake_minimum_required(VERSION 3.10 FATAL_ERROR) # CLI11 min requirement of CMake 3.10

project(
    arclaunch
    VERSION 0.0.1
    DESCRIPTION "arclaunch meta-project"
    LANGUAGES C CXX
)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 11)

# Submodules / dependencies
# add_subdirectory(lib/cli11)

# Cloudflare QUICHE (via corrosion)
include(cmake/quiche.cmake)

add_executable(ac_test main.cpp)

target_link_libraries(ac_test PUBLIC quiche-api)