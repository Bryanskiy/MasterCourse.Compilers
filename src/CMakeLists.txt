add_library(
    graph
    function.cc
    IR.cc
)

target_include_directories(graph PRIVATE ${PROJECT_SOURCE_DIR}/include/)
