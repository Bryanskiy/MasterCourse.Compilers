find_package(GTest REQUIRED)

set(TESTS
    graph_builder.cc
)

add_executable(tests ${TESTS})
target_link_libraries(tests graph GTest::gtest GTest::gtest_main pthread)
target_include_directories(tests PRIVATE ${PROJECT_SOURCE_DIR}/include/)
