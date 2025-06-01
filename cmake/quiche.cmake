add_subdirectory(lib/corrosion)
corrosion_import_crate(
    MANIFEST_PATH lib/quiche/Cargo.toml
    FEATURES ffi
)

add_library(quiche-api INTERFACE)
target_include_directories(quiche-api INTERFACE ${CMAKE_CURRENT_LIST_DIR}/../lib/quiche/quiche/include)
target_link_libraries(quiche-api INTERFACE quiche)

# Rust libraries must be installed using `corrosion_install`.
#corrosion_install(TARGETS quiche EXPORT RustLibTargets)