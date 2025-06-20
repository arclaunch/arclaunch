set(PROJECT arclaunch_physics)

project(
    ${PROJECT}
    VERSION ${CMAKE_PROJECT_VERSION}
    LANGUAGES CXX
)

add_library(${PROJECT} SHARED)

add_library(arclaunch::physics ALIAS ${PROJECT})

target_sources(${PROJECT}
    PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/src/jolt/bootstrap.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/jolt/provider.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/jolt/broad_phase.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/jolt/debug/recorder.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/simulation/base.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/simulation/plates.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/simulation/event/body.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/simulation/event/end.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/units.cpp
)


target_include_directories(${PROJECT}
     PRIVATE
          # internal headers
          ${CMAKE_CURRENT_SOURCE_DIR}/src
     PUBLIC
          # exported headers
          ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(${PROJECT} PUBLIC Jolt::Jolt Boost::signals2)

if (IS_TEST_ENV STREQUAL "true")
    add_subdirectory(test)
endif()