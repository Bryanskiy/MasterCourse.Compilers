#pragma once

#include <memory>

namespace jade {

template<typename T>
class IList;


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
    T* next() { return static_cast<T*>(m_next.get()); }
    T* prev() { return static_cast<T*>(m_prev); }
protected:
    friend class IList<T>;

    std::unique_ptr<T> m_next{nullptr};
    T* m_prev{nullptr};
};

template<typename T>
class IList {
public:
    // requires default T ctor
    T* append() {
        auto elem = std::make_unique<T>();
        if (m_start == nullptr) {
            m_last = elem.get();
            m_start = std::move(elem);
            return m_last;
        }

        elem->m_prev = m_last;
        m_last->m_next = std::move(elem);
        auto tmp = m_last->m_next.get();
        m_last = tmp;
        return m_last;
    }

private:
    std::unique_ptr<T> m_start{nullptr};
    T* m_last{nullptr};
};

} // namespace jade
