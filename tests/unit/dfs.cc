#include "gtest/gtest.h"
#include "dfs.hh"
#include "function.hh"

using namespace jade;

TEST(Dfs, Check) {
    ASSERT_TRUE(true);
}

//      +-----+
//      | bb0 |
//      +-----+
//         |
//         V
//      +-----+    +-----+
//   +->| bb1 |--->| bb2 |
//   |  +-----+    +-----+
//   |     |
//   |     V
//   |  +-----+
//   ---| bb3 |
//      +-----+
TEST(Dfs, fst) {
    // CREATE GRAPH
    auto function = Function{};

    auto bb0 = function.appendBB();
    auto bb1 = function.appendBB();
    auto bb2 = function.appendBB();
    auto bb3 = function.appendBB();

    bb0->addSuccessor(bb1);
    bb1->addSuccessor(bb2);
    bb1->addSuccessor(bb3);
    bb3->addSuccessor(bb1);

    // RUN DFS
    auto graph = function.getBasicBlocks();
    auto dfs = DFSIterator<BasicBlocksGraph>::begin(graph);
    ASSERT_EQ(*dfs, bb0);
    ASSERT_EQ(*(++dfs), bb1);
    ASSERT_EQ(*(++dfs), bb2);
    ASSERT_EQ(*(++dfs), bb3);
    ASSERT_EQ((++dfs), DFSIterator<BasicBlocksGraph>::end(graph));
}
