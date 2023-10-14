#pragma once

namespace jade {

template <typename GraphTy>
struct GraphTraits {
    using NodeTy = typename GraphTy::NodeTy;
    using EdgesItTy = typename GraphTy::EdgesItTy;

    static NodeTy entry(GraphTy& G);
    static EdgesItTy edgeBegin(NodeTy node);
    static EdgesItTy edgeEnd(NodeTy node);
};

} // namespace jade
