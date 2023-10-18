#pragma once

namespace jade {

template <typename GraphTy>
struct GraphTraits {
    using NodeTy = typename GraphTy::NodeTy;
    using EdgesItTy = typename GraphTy::EdgesItTy;

    static NodeTy entry(GraphTy& G);

    static EdgesItTy inEdgeBegin(NodeTy node);
    static EdgesItTy inEdgeEnd(NodeTy node);

    static EdgesItTy outEdgeBegin(NodeTy node);
    static EdgesItTy outEdgeEnd(NodeTy node);
};

} // namespace jade
