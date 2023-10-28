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

    domTree.dump(std::cout);

    auto range = graph.nodes();

    std::vector<BasicBlock*> bbs;
    for(auto it = range.begin(); it != range.end(); ++it) {
        bbs.push_back(&*it);
    }

    auto check = [&domTree](auto begin, auto end, BasicBlock* bb, bool mode) {
        for(; begin != end; ++begin) {
            ASSERT_EQ(domTree.dominate(bb, *begin), mode);
        }
    };

    {
        std::array bb0Dom = {
            bbs[0], bbs[1], bbs[2], bbs[3], bbs[4], bbs[5], bbs[6]
        };

        check(bb0Dom.begin(), bb0Dom.end(), bbs[0], true);
    }

    {
        std::array bb1Dom = {
           bbs[2], bbs[3], bbs[4], bbs[5], bbs[6], bbs[1]
        };

        check(bb1Dom.begin(), bb1Dom.end(), bbs[1], true);

        std::array bb1NDom = {
           bbs[0]
        };

        check(bb1NDom.begin(), bb1NDom.end(), bbs[1], false);
    }

    {
        std::array bb2Ndom = {
            bbs[0], bbs[1], bbs[3], bbs[4], bbs[5], bbs[6]
        };
        check(bb2Ndom.begin(), bb2Ndom.end(), bbs[2], false);
    }

    {
        std::array bb3Ndom = {
            bbs[0], bbs[1], bbs[2], bbs[4], bbs[5], bbs[6]
        };
        check(bb3Ndom.begin(), bb3Ndom.end(), bbs[3], false);
    }

    {
        std::array bb4 = {
            bbs[3], bbs[6], bbs[4]
        };
        check(bb4.begin(), bb4.end(), bbs[4], true);

        std::array bb4Ndom = {
            bbs[0], bbs[1], bbs[2], bbs[5]
        };
        check(bb4Ndom.begin(), bb4Ndom.end(), bbs[4], false);
    }

    {
        std::array bb5Ndom = {
            bbs[0], bbs[1], bbs[2], bbs[4], bbs[3], bbs[6]
        };
        check(bb5Ndom.begin(), bb5Ndom.end(), bbs[5], false);
    }

    {
        std::array bb6Ndom = {
            bbs[0], bbs[1], bbs[2], bbs[4], bbs[3], bbs[5]
        };
        check(bb6Ndom.begin(), bb6Ndom.end(), bbs[6], false);
    }
}

TEST(DomTree, example3) {
    auto function = example3();
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

    // {
    //     std::array bb0 = {
    //         bbs[1], bbs[2], bbs[3], bbs[4], bbs[5], bbs[6], bbs[7],
    //         bbs[8], bbs[9], bbs[10]
    //     };

    //     check(bb0.begin(), bb0.end(), bbs[0]);
    // }

    // {
    //     std::array bb1 = {
    //         bbs[2], bbs[3], bbs[4], bbs[5], bbs[6], bbs[7],
    //         bbs[8], bbs[9], bbs[10]
    //     };

    //     check(bb1.begin(), bb1.end(), bbs[1]);
    // }

    // {
    //     std::array bb2 = {
    //         bbs[3], bbs[4], bbs[5], bbs[6], bbs[7],
    //         bbs[8], bbs[9], bbs[10]
    //     };

    //     check(bb2.begin(), bb2.end(), bbs[2]);
    // }

    // {
    //     std::array bb3 = {
    //         bbs[4], bbs[5], bbs[6], bbs[7],
    //         bbs[8], bbs[9], bbs[10]
    //     };

    //     check(bb3.begin(), bb3.end(), bbs[3]);
    // }
}
