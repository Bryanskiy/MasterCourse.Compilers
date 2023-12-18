add_library(
    IR
    IR.cc
    function.cc
)

target_include_directories(IR
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
    PRIVATE ${PROJECT_SOURCE_DIR}/DSA
)
