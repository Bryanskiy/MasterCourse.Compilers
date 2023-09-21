#pragma once

#include <memory>

namespace jade {
// intrusive list:
//
//  +------+  next   +------+
//  | node | ------> | node |
//  +------+         +------+
//     |
//     | data, typeof(data) = T
//     V
//  +------+
//  | data |
//  +------+
template<typename T>
class IListNode {
public:
    using Self = IListNode<T>;
protected:

    std::unique_ptr<Self> m_next{nullptr};
    Self* m_prev{nullptr};
};

template<typename T>
class IList {
public:
    using Node = typename IListNode<T>::Self;

    // requires default T ctor
    T* append() {
        auto elem = std::make_unique<T>();
        if (m_start == nullptr) {
            m_start = std::move(elem);
            m_last = elem.get();
            return static_cast<T*>(m_last);
        }

        elem->m_prev = m_last;
        m_last->m_next = std::move(elem);
        auto tmp = m_last->m_next.get();
        m_last = tmp;
        return static_cast<T*>(m_last);
    }

private:
    friend class IListNode<T>;

    std::unique_ptr<Node> m_start{nullptr};
    Node* m_last{nullptr};
};

} // namespace jade
