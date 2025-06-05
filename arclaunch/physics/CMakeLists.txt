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