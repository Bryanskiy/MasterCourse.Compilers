#include "gtest/gtest.h"
#include <array>
#include "IR.hh"
#include "dfs.hh"
#include "function.hh"
#include "domTree.hh"
#include "graphs.hh"

using namespace jade;

TEST(DomTree, Check) {
    ASSERT_TRUE(true);
}

TEST(DomTree, example2) {
    auto function = example2();
    auto graph  = function.getBasicBlocks();
    auto builder = DominatorTreeBuilder<BasicBlocksGraph>();
    auto domTree = builder.build(graph);

    auto range = graph.nodes();

    std::vector<BasicBlock*> bbs;
    for(auto it = range.begin(); it != range.end(); ++it) {
        bbs.push_back(&*it);
    }

    auto check = [&domTree](auto begin, auto end, BasicBlock* bb) {
        for(; begin != end; ++begin) {
            ASSERT_EQ(domTree.dominate(bb, *begin), true);
        }
    };

    {
        std::array bb0 = {
            bbs[1], bbs[2], bbs[3], bbs[4], bbs[5], bbs[6]
        };

        check(bb0.begin(), bb0.end(), bbs[0]);
    }

    {
        std::array bb1 = {
           bbs[2], bbs[3], bbs[4], bbs[5], bbs[6]
        };

        check(bb1.begin(), bb1.end(), bbs[1]);
    }

    {
        std::array<BasicBlock*, 0> bb2 = {};
        check(bb2.begin(), bb2.end(), bbs[2]);
    }

    {
        std::array<BasicBlock*, 0> bb3 = {};
        check(bb3.begin(), bb3.end(), bbs[3]);
    }

    {
        std::array bb4 = {
            bbs[3], bbs[6]
        };
        check(bb4.begin(), bb4.end(), bbs[4]);
    }

    {
        std::array<BasicBlock*, 0> bb5 = {};
        check(bb5.begin(), bb5.end(), bbs[5]);
    }

    {
        std::array<BasicBlock*, 0> bb6 = {};
        check(bb6.begin(), bb6.end(), bbs[6]);
    }
}
