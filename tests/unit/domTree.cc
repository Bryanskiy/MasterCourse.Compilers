#include "gtest/gtest.h"
#include "dfs.hh"
#include "function.hh"
#include "domTree.hh"
#include "graphs.hh"

using namespace jade;

TEST(DomTree, Check) {
    ASSERT_TRUE(true);
}

TEST(DomTree, fst) {
    auto graph  = example2().getBasicBlocks();
    auto builder = DominatorTreeBuilder<BasicBlocksGraph>();
    auto domTree = builder.build(graph);
}
