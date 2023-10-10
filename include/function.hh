#pragma once

#include <memory>
#include <vector>

#include "IR.hh"
#include "ilist.hh"

namespace jade {

class Param : public Value, public IListNode {
public:
    Param(Type type) : Value{type} {}
};

class Function {
public:

    BasicBlock* appendBB() {
        auto* bb = new BasicBlock{};
        m_bbs.push_back(bb);
        return bb;
    }

    Param* appendParam(Type type) {
        auto* param = new Param{type};
        m_params.push_back(param);
        return param;
    }

    auto getBasicBlocks() { return Range{m_bbs.begin(), m_bbs.end()}; }

private:
    IList<BasicBlock> m_bbs;
    IList<Param> m_params;
};

} // namespace jade
