# GoogleTest section following
if (IS_TEST_ENV STREQUAL "true")

    set(PROJECT arclaunch_physics_test)

    project(${PROJECT})

    enable_testing()

    add_executable(${PROJECT}
        ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/plates_test.cpp
    )

    target_include_directories(${PROJECT}
        PRIVATE
            # internal headers
            ${CMAKE_CURRENT_SOURCE_DIR}/src/
    )

    target_link_libraries(${PROJECT} PUBLIC arclaunch::physics GTest::gtest_main)

    set_target_properties(${PROJECT} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/test-bin)
    include(GoogleTest)
    gtest_discover_tests(${PROJECT})
endif()