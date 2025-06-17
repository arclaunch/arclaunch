add_subdirectory(lib/corrosion)
corrosion_import_crate(
    MANIFEST_PATH lib/quiche/Cargo.toml
    FEATURES ffi
    CRATE_TYPES cdylib 
)

add_library(quiche-api INTERFACE)
target_include_directories(quiche-api INTERFACE ${CMAKE_CURRENT_LIST_DIR}/../lib/quiche/quiche/include)

corrosion_link_libraries(quiche ssl crypto)

target_link_libraries(quiche-api INTERFACE quiche)

if (WIN32)
    target_link_libraries(quiche-api INTERFACE crypt32)
endif()