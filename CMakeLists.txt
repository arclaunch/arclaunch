cmake_minimum_required(VERSION 3.10 FATAL_ERROR) # CLI11 min requirement of CMake 3.10

set(Rust_RUSTUP_INSTALL_MISSING_TARGET ON)

project(
    arclaunch
    VERSION 0.0.1
    DESCRIPTION "arclaunch meta-project"
    LANGUAGES C CXX
)

# init-statements in if; jolt
set(CMAKE_C_STANDARD 17)
set(CMAKE_CXX_STANDARD 17)

set(OPENSSL_NO_ASM TRUE)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# If using windows, make it a static binary to avoid errors around nor having libgcc[..].dll, libstdcc++[..].dll
if (WIN32)
    set(BUILD_SHARED_LIBS OFF)
    set(CMAKE_EXE_LINKER_FLAGS "-static") # for executable projects
    set(CMAKE_SHARED_LINKER_FLAGS "-static") # for library projects
endif()

add_compile_options(-Wno-error) # boringssl
add_subdirectory(lib/boringssl)

# Cloudflare QUICHE (via corrosion)
include(cmake/quiche.cmake)
corrosion_set_env_vars(quiche "QUICHE_BSSL_PATH=${CMAKE_CURRENT_BINARY_DIR}/lib/boringssl/")
corrosion_add_target_local_rustflags(quiche "-Clink-args=-lstdc++")

# CLI11
add_subdirectory(lib/cli11)


# Add Jolt
add_subdirectory(lib/joltphysics/Build)

# Add Boost
add_subdirectory(lib/boost)

# libevent
set(EVENT__DISABLE_OPENSSL ON)
add_subdirectory(lib/event)


add_library(uthash INTERFACE)
target_include_directories(uthash INTERFACE ${CMAKE_CURRENT_LIST_DIR}/lib/uthash/src)


add_executable(server)
target_sources(server
    PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/arclaunch/server/src/main.cpp
    ${CMAKE_CURRENT_LIST_DIR}/arclaunch/server/src/cli.cpp
    ${CMAKE_CURRENT_LIST_DIR}/arclaunch/server/src/quic/server.cpp
)
target_include_directories(server PUBLIC ${CMAKE_CURRENT_LIST_DIR}/arclaunch/server/include)

target_link_libraries(server PUBLIC quiche-api CLI11::CLI11 event_shared uthash arclaunch::physics)


include(ExternalProject)
ExternalProject_Add(
    jolt_ext
    PREFIX ${CMAKE_BINARY_DIR}/lib/joltphysics-external/Build
    SOURCE_DIR ${CMAKE_SOURCE_DIR}/lib/joltphysics/Build
    CMAKE_ARGS -DTARGET_PERFORMANCE_TEST=ON -DTARGET_VIEWER=ON -DENABLE_INSTALL=OFF -DCMAKE_BUILD_TYPE=Release
    INSTALL_COMMAND ""
)


add_subdirectory(arclaunch/physics)