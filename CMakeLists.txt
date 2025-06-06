cmake_minimum_required(VERSION 3.10 FATAL_ERROR) # CLI11 min requirement of CMake 3.10

set(Rust_RUSTUP_INSTALL_MISSING_TARGET ON CACHE BOOL "Install missing rustup targets automatically")

if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/install" CACHE STRING "install prefix" FORCE)
endif()

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


set(BUILD_SHARED_LIBS OFF)
add_compile_options(-Wno-error) # boringssl
add_subdirectory(lib/boringssl EXCLUDE_FROM_ALL)
set(BUILD_SHARED_LIBS ON)

# If using windows, make it a static binary to avoid errors around nor having libgcc[..].dll, libstdcc++[..].dll
if (WIN32)
    set(INTERPROCEDURAL_OPTIMIZATION OFF)
    set(CMAKE_EXE_LINKER_FLAGS "-Wl,-allow-multiple-definition -static-libgcc -static-libstdc++") # for executable projects
    set(CMAKE_SHARED_LINKER_FLAGS "-Wl,-allow-multiple-definition -static-libgcc -static-libstdc++") # for library projects

    # prevent "try_run() invoked in cross-compiling mode" error from CMake
    set(BOOST_CHARCONV_QUADMATH_FOUND FALSE CACHE BOOL "boost charconv could find quadmath")
endif()


# Cloudflare QUICHE (via corrosion)
include(cmake/quiche.cmake)
corrosion_set_env_vars(quiche "QUICHE_BSSL_PATH=${CMAKE_CURRENT_BINARY_DIR}/lib/boringssl/")
if(NOT ANDROID)
    corrosion_add_target_local_rustflags(quiche "-Clink-args=-static-libgcc -static-libstdc++")
    corrosion_set_linker(quiche ${CMAKE_CXX_COMPILER}) # so that -static-libstdc++ flag is understood <https://github.com/msys2/MINGW-packages/issues/6163#issuecomment-2094285343>
endif()

# CLI11
add_subdirectory(lib/cli11 EXCLUDE_FROM_ALL)


# Add Jolt
add_subdirectory(lib/joltphysics/Build EXCLUDE_FROM_ALL)

# Add Boost
add_subdirectory(lib/boost EXCLUDE_FROM_ALL)

# libevent
set(EVENT__DISABLE_OPENSSL ON)
add_subdirectory(lib/event EXCLUDE_FROM_ALL)


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

target_link_libraries(server PUBLIC quiche-api CLI11::CLI11 event_core_shared event_shared uthash arclaunch::physics)


include(ExternalProject)
ExternalProject_Add(
    jolt_ext
    PREFIX ${CMAKE_BINARY_DIR}/lib/joltphysics-external/Build
    SOURCE_DIR ${CMAKE_SOURCE_DIR}/lib/joltphysics/Build
    CMAKE_ARGS -DTARGET_PERFORMANCE_TEST=ON -DTARGET_VIEWER=ON -DENABLE_INSTALL=OFF -DCMAKE_BUILD_TYPE=Release
    INSTALL_COMMAND ""
)
set_target_properties(jolt_ext PROPERTIES EXCLUDE_FROM_ALL TRUE)


add_subdirectory(arclaunch/physics)
add_subdirectory(arclaunch/unity)



set(CPACK_COMPONENTS_ALL "")
macro(install_component name)
    set(targets ${ARGV})
    list(REMOVE_AT targets 0)
    string(STRIP "${name}" name_strip)
    install(TARGETS ${targets}
        COMPONENT "${name_strip}"
        # RUNTIME_DEPENDENCIES
        RUNTIME DESTINATION ${name_strip}
        LIBRARY DESTINATION ${name_strip}
    )
    if (CPACK_COMPONENTS_ALL STREQUAL "")
        set(CPACK_COMPONENTS_ALL "${name_strip}")
    else()
        set(CPACK_COMPONENTS_ALL "${CPACK_COMPONENTS_ALL};${name_strip}")
    endif()
    message("Added component ${name_strip} with targets '${targets}'")
endmacro()


install_component(unity arclaunch_unity arclaunch_physics Jolt)
#install_component(server server arclaunch_physics Jolt event_core_shared)

# TODO: quiche not included in server build...
#corrosion_install(TARGETS quiche DESTINATION server COMPONENT server)

# Common CPack settings

string(TOLOWER "${PROJECT_NAME}-${CMAKE_SYSTEM_NAME}" CPACK_PACKAGE_FILE_NAME)

# cmake components are default ${CMAKE_PACKAGE_FILE_NAME}-<component>

set(CPACK_INCLUDE_TOPLEVEL_DIRECTORY OFF)
set(CPACK_COMPONENT_INCLUDE_TOPLEVEL_DIRECTORY OFF)
set(CPACK_GENERATOR "ZIP")
set(CPACK_COMPONENTS_GROUPING ONE_PER_GROUP)
set(CPACK_ARCHIVE_COMPONENT_INSTALL ON)
set(CPACK_PACKAGE_DIRECTORY ${CMAKE_BINARY_DIR}/pkg)

include(CPack)