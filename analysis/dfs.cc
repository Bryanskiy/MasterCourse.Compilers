#include "dfs.hh"

namespace jade {

void DFS::run() {
    reset();

    auto bbs = m_graph->getBasicBlocks();
    auto source = bbs.begin().getPtr();
    runInner(source);
}

void DFS::runInner(BasicBlock* bb) {
    m_visited[bb] = GREEN;

    for(auto&& succs : bb->successors()) {
        if (m_visited[succs] == GREEN) {
            continue;
        }

        runInner(succs);
    }
}

} // namespace jade
