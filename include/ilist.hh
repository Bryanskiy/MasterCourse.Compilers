#pragma once

#include <cassert>
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
    T* next() { return m_next.get(); }
    T* prev() { return m_prev; }
protected:

    std::unique_ptr<T> m_next{nullptr};
    T* m_prev{nullptr};

private:
    friend class IList<T>;

    // change owner
    std::unique_ptr<T> next_unique() { return std::move(m_next); } // TODO: is it safe?
};

template<typename T>
class IList {
public:

    // T is a IListNode<T>
    template<typename... Args>
    T* insert(T* const inserter, Args&&... args) {
        auto elem = std::unique_ptr<T>(new T(std::forward<Args>(args)...));

        if (inserter == nullptr) {
            assert(m_start == nullptr);

            m_last = elem.get();
            m_start = std::move(elem);
            return m_last;
        }

        auto next = std::move(inserter->next_unique());
        elem->m_prev = inserter;
        inserter->m_next = std::move(elem);

        if (next == nullptr) {
            assert(inserter == m_last);
            m_last = inserter->m_next.get();
        } else {
            inserter->m_next->m_next = std::move(next);
            next->m_prev = inserter->m_next.get();
        }

        return inserter->m_next.get();
    }

    T* append() {
        return insert(m_last);
    }

private:
    std::unique_ptr<T> m_start{nullptr};
    T* m_last{nullptr};
};

} // namespace jade
