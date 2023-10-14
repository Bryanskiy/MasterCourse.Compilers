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
    bb1->addPredecessor(bb0);
    bb1->addPredecessor(bb3);

    bb2->addPredecessor(bb1);

    bb3->addPredecessor(bb1);
    bb3->addSuccessor(bb1);

    // RUN DFS
    auto graph = function.getBasicBlocks();
    auto dfs = DFSIterator<BasicBlocksGraph>::begin(graph);
}
