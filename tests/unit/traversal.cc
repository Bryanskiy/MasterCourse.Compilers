#include "gtest/gtest.h"
#include <vector>
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

TEST(Dfs, example3) {
    auto function = example3();
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
    ASSERT_EQ(*(++dfs), bbs[4]);
    ASSERT_EQ(*(++dfs), bbs[5]);
    ASSERT_EQ(*(++dfs), bbs[6]);
    ASSERT_EQ(*(++dfs), bbs[7]);
    ASSERT_EQ(*(++dfs), bbs[8]);
    ASSERT_EQ(*(++dfs), bbs[9]);
    ASSERT_EQ(*(++dfs), bbs[10]);
    ASSERT_EQ((++dfs), DFSIterator<BasicBlocksGraph>::end(graph));
}

TEST(Dfs, example4) {
    auto function = example4();
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
    ASSERT_EQ(*(++dfs), bbs[4]);
    ASSERT_EQ(*(++dfs), bbs[5]);
    ASSERT_EQ(*(++dfs), bbs[8]);
    ASSERT_EQ(*(++dfs), bbs[7]);
    ASSERT_EQ(*(++dfs), bbs[6]);
    ASSERT_EQ((++dfs), DFSIterator<BasicBlocksGraph>::end(graph));
}

TEST(PO, example1) {
    auto function = example1();
    auto graph = function.getBasicBlocks();
    auto range = graph.nodes();

    std::vector<BasicBlock*> bbs;
    for(auto it = range.begin(); it != range.end(); ++it) {
        bbs.push_back(&*it);
    }

    auto po = PostOrderIterator<BasicBlocksGraph>::begin(graph);
    ASSERT_EQ(*po, bbs[2]);
    ASSERT_EQ(*(++po), bbs[3]);
    ASSERT_EQ(*(++po), bbs[1]);
    ASSERT_EQ(*(++po), bbs[0]);
    ASSERT_EQ((++po), PostOrderIterator<BasicBlocksGraph>::end(graph));
}

TEST(PO, example2) {
    auto function = example2();
    auto graph = function.getBasicBlocks();
    auto range = graph.nodes();

    std::vector<BasicBlock*> bbs;
    for(auto it = range.begin(); it != range.end(); ++it) {
        bbs.push_back(&*it);
    }

    auto po = PostOrderIterator<BasicBlocksGraph>::begin(graph);
    ASSERT_EQ(*po, bbs[5]);
    ASSERT_EQ(*(++po), bbs[2]);
    ASSERT_EQ(*(++po), bbs[3]);
    ASSERT_EQ(*(++po), bbs[6]);
    ASSERT_EQ(*(++po), bbs[4]);
    ASSERT_EQ(*(++po), bbs[1]);
    ASSERT_EQ(*(++po), bbs[0]);
    ASSERT_EQ((++po), PostOrderIterator<BasicBlocksGraph>::end(graph));
}

TEST(PO, example3) {
    auto function = example3();
    auto graph = function.getBasicBlocks();
    auto range = graph.nodes();

    std::vector<BasicBlock*> bbs;
    for(auto it = range.begin(); it != range.end(); ++it) {
        bbs.push_back(&*it);
    }

    auto po = PostOrderIterator<BasicBlocksGraph>::begin(graph);
    ASSERT_EQ(*po, bbs[8]);
    ASSERT_EQ(*(++po), bbs[7]);
    ASSERT_EQ(*(++po), bbs[9]);
    ASSERT_EQ(*(++po), bbs[6]);
    ASSERT_EQ(*(++po), bbs[5]);
    ASSERT_EQ(*(++po), bbs[4]);
    ASSERT_EQ(*(++po), bbs[3]);
    ASSERT_EQ(*(++po), bbs[2]);
    ASSERT_EQ(*(++po), bbs[10]);
    ASSERT_EQ(*(++po), bbs[1]);
    ASSERT_EQ(*(++po), bbs[0]);
    ASSERT_EQ((++po), PostOrderIterator<BasicBlocksGraph>::end(graph));
}

TEST(PO, example4) {
    auto function = example4();
    auto graph = function.getBasicBlocks();
    auto range = graph.nodes();

    std::vector<BasicBlock*> bbs;
    for(auto it = range.begin(); it != range.end(); ++it) {
        bbs.push_back(&*it);
    }

    auto po = PostOrderIterator<BasicBlocksGraph>::begin(graph);
    ASSERT_EQ(*po, bbs[5]);
    ASSERT_EQ(*(++po), bbs[4]);
    ASSERT_EQ(*(++po), bbs[3]);
    ASSERT_EQ(*(++po), bbs[2]);
    ASSERT_EQ(*(++po), bbs[6]);
    ASSERT_EQ(*(++po), bbs[7]);
    ASSERT_EQ(*(++po), bbs[8]);
    ASSERT_EQ(*(++po), bbs[1]);
    ASSERT_EQ(*(++po), bbs[0]);
    ASSERT_EQ((++po), PostOrderIterator<BasicBlocksGraph>::end(graph));
}

// RPO delegates to PO => 1 test is enough
TEST(RPO, example1) {
    auto function = example1();
    auto graph = function.getBasicBlocks();
    auto range = graph.nodes();

    std::vector<BasicBlock*> bbs;
    for(auto it = range.begin(); it != range.end(); ++it) {
        bbs.push_back(&*it);
    }

    auto po = RPOIterator<BasicBlocksGraph>::begin(graph);
    ASSERT_EQ(*po, bbs[0]);
    ASSERT_EQ(*(++po), bbs[1]);
    ASSERT_EQ(*(++po), bbs[3]);
    ASSERT_EQ(*(++po), bbs[2]);
    ASSERT_EQ((++po), RPOIterator<BasicBlocksGraph>::end(graph));
}
