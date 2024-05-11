add_library(
    passes
    constFolding.cc
    dce.cc
    peepholes.cc
    inline.cc
    checksElimination.cc
)

target_link_libraries(passes IR)
target_include_directories(passes
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
    PRIVATE ${PROJECT_SOURCE_DIR}/DSA
    PRIVATE ${PROJECT_SOURCE_DIR}/analysis
    PRIVATE ${PROJECT_SOURCE_DIR}/IR
)
