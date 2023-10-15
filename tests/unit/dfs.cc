#include "gtest/gtest.h"
#include <vector>
#include "dfs.hh"
#include "graphs.hh"
#include "function.hh"

using namespace jade;


TEST(Dfs, Check) {
    ASSERT_TRUE(true);
}

// graphs.hh - example 1
TEST(Dfs, example1G) {
    auto function = example1();
    auto graph = function.getBasicBlocks();
    auto range = graph.nodes();

    std::vector<BasicBlock*> bbs;
    for(auto it = range.begin(); it != range.end(); ++it) {
        bbs.push_back(&*it);
    }

    auto dfs = DFSIterator<BasicBlocksGraph>::begin(graph);

    ASSERT_EQ(*dfs, bbs[0]);
    ASSERT_EQ(*(++dfs), bbs[1]);
    ASSERT_EQ(*(++dfs), bbs[2]);
    ASSERT_EQ(*(++dfs), bbs[3]);
    ASSERT_EQ((++dfs), DFSIterator<BasicBlocksGraph>::end(graph));
}

// graphs.hh - example 2
TEST(Dfs, example2G) {
    auto function = example2();
    auto graph = function.getBasicBlocks();

    auto range = graph.nodes();

    std::vector<BasicBlock*> bbs;
    for(auto it = range.begin(); it != range.end(); ++it) {
        bbs.push_back(&*it);
    }

    auto dfs = DFSIterator<BasicBlocksGraph>::begin(graph);

    ASSERT_EQ(*dfs, bbs[0]);
    ASSERT_EQ(*(++dfs), bbs[1]);
    ASSERT_EQ(*(++dfs), bbs[2]);
    ASSERT_EQ(*(++dfs), bbs[5]);
    ASSERT_EQ(*(++dfs), bbs[4]);
    ASSERT_EQ(*(++dfs), bbs[3]);
    ASSERT_EQ(*(++dfs), bbs[6]);
    ASSERT_EQ((++dfs), DFSIterator<BasicBlocksGraph>::end(graph));
}
