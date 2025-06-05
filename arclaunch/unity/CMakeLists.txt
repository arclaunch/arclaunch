set(PROJECT arclaunch_unity)

project(
    ${PROJECT}
    VERSION ${CMAKE_PROJECT_VERSION}
    LANGUAGES CXX
)

add_library(${PROJECT} SHARED)

add_library(arclaunch::unity ALIAS ${PROJECT})

target_sources(${PROJECT}
    PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/src/main.cpp
)


target_include_directories(${PROJECT}
     PRIVATE
          # internal headers
          ${CMAKE_CURRENT_SOURCE_DIR}/src
     PUBLIC
          # exported headers
          ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(${PROJECT} PUBLIC arclaunch::physics)

function (copy_dlls target)
    if (WIN32)
        add_custom_command (
            TARGET "${target}" POST_BUILD
            COMMAND "${CMAKE_COMMAND}" -E copy -t "$<TARGET_FILE_DIR:${target}>"
                    "$<TARGET_RUNTIME_DLLS:${target}>" USES_TERMINAL COMMAND_EXPAND_LISTS
        )
    endif ()
endfunction ()

copy_dlls(arclaunch_unity)
