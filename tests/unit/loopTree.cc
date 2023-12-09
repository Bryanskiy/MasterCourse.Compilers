#include "gtest/gtest.h"
#include <array>
#include <iostream>
#include <vector>
#include "IR.hh"
#include "function.hh"
#include "graphs.hh"
#include "loopAnalyser.hh"

using namespace jade;

using Loop = LoopTreeNode<BasicBlocksGraph>;

bool checkBackEdges(Loop* loop, std::set<BasicBlock*> expected) {
    auto actual = loop->getBackEdges();
    return actual == expected;
}

bool checkBody(Loop* loop, std::set<BasicBlock*> expected) {
    auto actual = loop->getNodes();
    return actual == expected;
}

bool checkInnerLoops(Loop* loop, std::set<Loop*> expected) {
    auto actual = loop->getInners();
    return actual == expected;
}

TEST(LoopTree, Check) {
    ASSERT_TRUE(true);
}

TEST(LoopTree, Example1) {
    auto function = example1();
    auto graph  = function.getBasicBlocks();

    auto range = graph.nodes();
    std::vector<BasicBlock*> bbs;
    for(auto it = range.begin(); it != range.end(); ++it) {
        bbs.push_back(&*it);
    }

    auto builder = LoopTreeBuilder<BasicBlocksGraph>();
    auto loopTree = builder.build(graph);

    auto loop = loopTree.getLoop(bbs[1]);
    ASSERT_EQ(loop->getHeader(), bbs[1]);
    ASSERT_EQ(loop->getOuter(), nullptr);
    ASSERT_EQ(loop->isReducible(), true);
    ASSERT_TRUE(checkBackEdges(loop, {bbs[3]}));
    ASSERT_TRUE(checkBody(loop, {bbs[3]}));
    ASSERT_EQ(loopTree.getLoop(bbs[0]), nullptr);
    ASSERT_EQ(loopTree.getLoop(bbs[2]), nullptr);
}

TEST(LoopTree, Example2) {
    auto function = example2();
    auto graph  = function.getBasicBlocks();

    auto range = graph.nodes();
    std::vector<BasicBlock*> bbs;
    for(auto it = range.begin(); it != range.end(); ++it) {
        bbs.push_back(&*it);
    }

    auto builder = LoopTreeBuilder<BasicBlocksGraph>();
    auto loopTree = builder.build(graph);
    for(auto* bb: bbs) {
        ASSERT_EQ(loopTree.getLoop(bb), nullptr);
    }
}

TEST(LoopTree, Example3) {
    auto function = example3();
    auto graph  = function.getBasicBlocks();

    auto range = graph.nodes();
    std::vector<BasicBlock*> bbs;
    for(auto it = range.begin(); it != range.end(); ++it) {
        bbs.push_back(&*it);
    }

    auto builder = LoopTreeBuilder<BasicBlocksGraph>();
    auto loopTree = builder.build(graph);
    {
        auto loop = loopTree.getLoop(bbs[1]);
        ASSERT_EQ(loop->getHeader(), bbs[1]);
        ASSERT_EQ(loop->getOuter(), nullptr);
        ASSERT_EQ(loop->isReducible(), true);
        ASSERT_TRUE(checkBackEdges(loop, {bbs[9]}));
        ASSERT_TRUE(checkBody(loop, {bbs[9], bbs[10], bbs[6]}));
        ASSERT_TRUE(checkInnerLoops(loop, {loopTree.getLoop(bbs[4]), loopTree.getLoop(bbs[2])}));
    }

    {
        auto loop = loopTree.getLoop(bbs[3]);
        ASSERT_EQ(loop->getHeader(), bbs[2]);
        ASSERT_EQ(loop->getOuter(), loopTree.getLoop(bbs[1]));
        ASSERT_EQ(loop->isReducible(), true);
        ASSERT_TRUE(checkBackEdges(loop, {bbs[3]}));
        ASSERT_TRUE(checkBody(loop, {bbs[3]}));
    }

    {
        auto loop = loopTree.getLoop(bbs[5]);
        ASSERT_EQ(loop->getHeader(), bbs[4]);
        ASSERT_EQ(loop->getOuter(), loopTree.getLoop(bbs[1]));
        ASSERT_EQ(loop->isReducible(), true);
        ASSERT_TRUE(checkBackEdges(loop, {bbs[5]}));
        ASSERT_TRUE(checkBody(loop, {bbs[5]}));
    }
}

TEST(LoopTree, Example4) {
    auto function = example4();
    auto graph  = function.getBasicBlocks();

    auto range = graph.nodes();
    std::vector<BasicBlock*> bbs;
    for(auto it = range.begin(); it != range.end(); ++it) {
        bbs.push_back(&*it);
    }

    auto builder = LoopTreeBuilder<BasicBlocksGraph>();
    auto loopTree = builder.build(graph);
    {
        // irreducible loop
        auto loop = loopTree.getLoop(bbs[2]);
        ASSERT_EQ(loop->getHeader(), bbs[2]);
        ASSERT_EQ(loop->getOuter(), nullptr);
        ASSERT_EQ(loop->isReducible(), false);
        ASSERT_TRUE(checkBackEdges(loop, {bbs[4]}));
    }

    {
        auto loop = loopTree.getLoop(bbs[1]);
        ASSERT_EQ(loop->getHeader(), bbs[1]);
        ASSERT_EQ(loop->getOuter(), nullptr);
        ASSERT_EQ(loop->isReducible(), true);
        ASSERT_TRUE(checkBackEdges(loop, {bbs[7]}));
        ASSERT_TRUE(checkBody(loop, {bbs[7], bbs[8]}));
    }
}

TEST(LoopTree, Example5) {
    auto function = example5();
    auto graph  = function.getBasicBlocks();

    auto range = graph.nodes();
    std::vector<BasicBlock*> bbs;
    for(auto it = range.begin(); it != range.end(); ++it) {
        bbs.push_back(&*it);
    }

    auto builder = LoopTreeBuilder<BasicBlocksGraph>();
    auto loopTree = builder.build(graph);
    {
        auto loop = loopTree.getLoop(bbs[1]);
        ASSERT_EQ(loop->getHeader(), bbs[1]);
        ASSERT_EQ(loop->getOuter(), nullptr);
        ASSERT_EQ(loop->isReducible(), true);
        ASSERT_TRUE(checkBackEdges(loop, {bbs[3]}));
        ASSERT_TRUE(checkBody(loop, {bbs[3], bbs[4]}));
        ASSERT_TRUE(checkInnerLoops(loop, {}));
    }
}

TEST(LoopTree, Example6) {
    auto function = example6();
    auto graph  = function.getBasicBlocks();

    auto range = graph.nodes();
    std::vector<BasicBlock*> bbs;
    for(auto it = range.begin(); it != range.end(); ++it) {
        bbs.push_back(&*it);
    }

    auto builder = LoopTreeBuilder<BasicBlocksGraph>();
    auto loopTree = builder.build(graph);
    {
        auto loop = loopTree.getLoop(bbs[1]);
        ASSERT_EQ(loop->getHeader(), bbs[1]);
        ASSERT_EQ(loop->getOuter(), nullptr);
        ASSERT_EQ(loop->isReducible(), true);
        ASSERT_TRUE(checkBackEdges(loop, {bbs[4]}));
        ASSERT_TRUE(checkBody(loop, {bbs[2], bbs[3], bbs[4]}));
        ASSERT_TRUE(checkInnerLoops(loop, {}));
    }
}

TEST(LoopTree, Example7) {
    auto function = example7();
    auto graph  = function.getBasicBlocks();

    auto range = graph.nodes();
    std::vector<BasicBlock*> bbs;
    for(auto it = range.begin(); it != range.end(); ++it) {
        bbs.push_back(&*it);
    }

    auto builder = LoopTreeBuilder<BasicBlocksGraph>();
    auto loopTree = builder.build(graph);

    {
        auto loop = loopTree.getLoop(bbs[0]);
        ASSERT_EQ(loop->getHeader(), bbs[0]);
        ASSERT_EQ(loop->getOuter(), nullptr);
        ASSERT_EQ(loop->isReducible(), true);
        ASSERT_TRUE(checkBackEdges(loop, {bbs[7]}));
        ASSERT_TRUE(checkBody(loop, {bbs[7]}));
        ASSERT_TRUE(checkInnerLoops(loop, {loopTree.getLoop(bbs[1])}));
    }

    {
        auto loop = loopTree.getLoop(bbs[1]);
        ASSERT_EQ(loop->getHeader(), bbs[1]);
        ASSERT_EQ(loop->getOuter(), loopTree.getLoop(bbs[0]));
        ASSERT_EQ(loop->isReducible(), true);
        ASSERT_TRUE(checkBackEdges(loop, {bbs[6]}));
        ASSERT_TRUE(checkBody(loop, {bbs[4], bbs[5], bbs[2], bbs[6]}));
        ASSERT_TRUE(checkInnerLoops(loop, {}));
    }
}
