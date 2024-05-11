add_library(
    analysis
    liveness.cc
    domTree.cc
)

target_include_directories(analysis
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
    PRIVATE ${PROJECT_SOURCE_DIR}/DSA
    PRIVATE ${PROJECT_SOURCE_DIR}/IR
)
