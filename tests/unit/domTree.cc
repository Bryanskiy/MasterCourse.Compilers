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
    auto function = example2();
    auto graph  = function.getBasicBlocks();
    auto builder = DominatorTreeBuilder<BasicBlocksGraph>();
    auto domTree = builder.build(graph);

    auto range = graph.nodes();

    std::vector<BasicBlock*> bbs;
    for(auto it = range.begin(); it != range.end(); ++it) {
        bbs.push_back(&*it);
    }

    ASSERT_EQ(domTree.dominate(bbs[0], bbs[1]), true);
}
