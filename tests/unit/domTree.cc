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

}
