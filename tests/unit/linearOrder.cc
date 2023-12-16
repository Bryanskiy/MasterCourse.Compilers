#include <array>

#include "gtest/gtest.h"
#include <iostream>
#include <vector>
#include "graphs.hh"
#include "linearOrder.hh"
#include "IR.hh"
#include "function.hh"

using namespace jade;

TEST(LinearOrder, Check) {
    ASSERT_TRUE(true);
}

bool checkOrder(const std::vector<BasicBlock*> lhs, std::vector<BasicBlock*> rhs) {
    return lhs == rhs;
}

TEST(LinearOrder, Example1) {
    auto function = example1();
    auto graph  = function.getBasicBlocks();
    auto builder = LinearOrder(graph);

    auto range = graph.nodes();
    std::vector<BasicBlock*> bbs;
    for(auto it = range.begin(); it != range.end(); ++it) {
        bbs.push_back(&*it);
    }

    auto actual = builder.linearize();
    ASSERT_TRUE(checkOrder(actual, {bbs[0], bbs[1], bbs[3], bbs[2]}));
}
