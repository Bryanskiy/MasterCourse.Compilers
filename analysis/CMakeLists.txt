add_library(
    analysis
    linearOrder.cc
)

target_include_directories(analysis
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}

    PRIVATE ${PROJECT_SOURCE_DIR}/IR
    PRIVATE ${PROJECT_SOURCE_DIR}/DSA
)
