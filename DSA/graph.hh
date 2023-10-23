#pragma once

#include <cstddef>

namespace jade {

template <typename GraphTy>
struct GraphTraits {
    using NodeTy = typename GraphTy::NodeTy;
    using NodesIt = typename GraphTy::NodesIt;
    using EdgesItTy = typename GraphTy::EdgesItTy;

    static NodeTy entry(const GraphTy& G);
    static NodesIt nodesBegin(GraphTy& G);
    static NodesIt nodesEnd(GraphTy& G);

    static std::size_t nodesCount(const GraphTy& G);
    static std::size_t id(NodeTy node);


    static EdgesItTy inEdgeBegin(NodeTy node);
    static EdgesItTy inEdgeEnd(NodeTy node);

    static EdgesItTy outEdgeBegin(NodeTy node);
    static EdgesItTy outEdgeEnd(NodeTy node);
};

} // namespace jade
