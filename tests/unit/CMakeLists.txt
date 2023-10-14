find_package(GTest REQUIRED)

set(TESTS
    graphBuilder.cc
)

add_executable(tests ${TESTS})
target_link_libraries(tests analysis GTest::gtest GTest::gtest_main pthread)
target_include_directories(tests
    PRIVATE ${PROJECT_SOURCE_DIR}/analysis
    PRIVATE ${PROJECT_SOURCE_DIR}/IR
    PRIVATE ${PROJECT_SOURCE_DIR}/DSA
)
