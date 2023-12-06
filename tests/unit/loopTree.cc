#include "gtest/gtest.h"
#include <array>
#include <iostream>
#include "IR.hh"
#include "function.hh"
#include "graphs.hh"
#include "loopAnalyser.hh"

using namespace jade;

TEST(LoopTree, Check) {
    ASSERT_TRUE(true);
}

TEST(LoopTree, Example1) {
    auto function = example1();
    auto graph  = function.getBasicBlocks();
    auto builder = LoopTreeBuilder<BasicBlocksGraph>();
    auto loopTree = builder.build(graph);
}
