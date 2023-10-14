#pragma once

#include <unordered_map>

#include "PM.hh"
#include "../IR/IR.hh"

namespace jade {

class Function;

class DFS : public Pass {
public:
    DFS(Function* graph) : Pass{graph} {}

    virtual std::string name() const override { return "DFS"; }
    virtual void run() override;

    void reset() { m_visited.clear(); }
private:
    enum Color {
        RED = 0,
        GREEN // visited
    };

    void runInner(BasicBlock* current);
    std::unordered_map<BasicBlock*, Color> m_visited;
};

} // namespace jade
