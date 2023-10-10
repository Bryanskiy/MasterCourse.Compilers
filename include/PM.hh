#pragma once

#include "function.hh"
#include <memory>
#include <string_view>
#include <vector>
namespace jade {

class Pass {
public:
    Pass(Function* graph) : m_graph{graph} {}

    virtual std::string_view name() const;
    virtual void run() = 0;
protected:
    Function* m_graph;
};

class PassManager {
public:

    template<typename PassT, typename... Args>
    void registerPass(Args&&... args) {
        static_assert(std::is_base_of_v<Pass, PassT>, "invalid type during pass creation");
        auto pass = std::make_unique<PassT>(std::forward<Args>(args)...);
        m_passes.emplace_back(pass);
    }

    void run() {
        for (auto&& pass : m_passes) {
            pass->run();
        }
    }
private:
    std::vector<std::unique_ptr<Pass>> m_passes;
};

} // namespace jade
