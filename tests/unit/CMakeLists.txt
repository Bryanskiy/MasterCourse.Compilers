find_package(GTest REQUIRED)

set(TESTS
    graphBuilder.cc
    traversal.cc
    domTree.cc
    graphs.cc
    loopTree.cc
    linearOrder.cc
    liveness.cc
)

add_executable(tests ${TESTS})
target_link_libraries(tests analysis GTest::gtest GTest::gtest_main pthread)
target_include_directories(tests
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
    PRIVATE ${PROJECT_SOURCE_DIR}/analysis
    PRIVATE ${PROJECT_SOURCE_DIR}/IR
    PRIVATE ${PROJECT_SOURCE_DIR}/DSA
)
