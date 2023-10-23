#include "graphs.hh"
#include "IR.hh"
#include <array>

Function example1() {
    auto function = Function{};

    std::array<BasicBlock*, 4> bbs;
    for(std::size_t i = 0; i < bbs.size(); ++i) {
        bbs[i] = function.create<BasicBlock>();
    }

    bbs[0]->addSuccessor(bbs[1]);
    bbs[1]->addSuccessor(bbs[2]);
    bbs[1]->addSuccessor(bbs[3]);
    bbs[3]->addSuccessor(bbs[1]);

    return function;
}

Function example2() {
    auto function = Function{};

    std::array<BasicBlock*, 7> bbs;
    for(std::size_t i = 0; i < bbs.size(); ++i) {
        bbs[i] = function.create<BasicBlock>();
    }

    bbs[0]->addSuccessor(bbs[1]);

    bbs[1]->addSuccessor(bbs[2]);
    bbs[1]->addSuccessor(bbs[4]);

    bbs[2]->addSuccessor(bbs[5]);

    bbs[3]->addSuccessor(bbs[5]);

    bbs[4]->addSuccessor(bbs[3]);
    bbs[4]->addSuccessor(bbs[6]);

    bbs[6]->addSuccessor(bbs[5]);

    return function;
}
