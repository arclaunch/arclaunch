cmake_minimum_required(VERSION 3.10 FATAL_ERROR) # CLI11 min requirement of CMake 3.10

set(Rust_RUSTUP_INSTALL_MISSING_TARGET ON)

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


add_library(demo SHARED src/main.cpp)
#target_include_directories(demo PUBLIC ${CMAKE_CURRENT_LIST_DIR}/include)

target_link_libraries(demo PUBLIC quiche-api)