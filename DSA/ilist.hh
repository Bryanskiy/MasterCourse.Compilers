#pragma once

#include <cassert>
#include <iterator>
#include <memory>
#include <type_traits>
#include <utility>

namespace jade {

class IListNode {
    IListNode* m_next{nullptr};
    IListNode* m_prev{nullptr};

public:
    IListNode* getNext() { return m_next; }
    IListNode* getPrev() { return m_prev; }

    void setNext(IListNode* next) { m_next = next; }
    void setPrev(IListNode* prev) { m_prev = prev; }
};

struct IListBase {
    void remove(IListNode* marker) {
        auto* prev = marker->getPrev();
        auto* next = marker->getNext();

        next->setPrev(prev);
        prev->setNext(next);

        marker->setNext(nullptr);
        marker->setPrev(nullptr);
    }

    void insertBeforeBase(IListNode* inserter, IListNode* elem) {
        assert(inserter);
        assert(elem);

        auto* prev = inserter->getPrev();
        if (prev) {
            prev->setNext(elem);
        }
        inserter->setPrev(elem);

        elem->setNext(inserter);
        elem->setPrev(prev);
    }

    void insertAfterBase(IListNode* inserter, IListNode* elem) {
        assert(inserter);
        assert(elem);

        auto* next = inserter->getNext();
        if (next) {
            next->setPrev(elem);
        }
        inserter->setNext(elem);

        elem->setNext(next);
        elem->setPrev(inserter);
    }
};

template<typename NodeTy>
class IListIterator {
public:
    using iterator_category = std::bidirectional_iterator_tag;
    using difference_type = std::ptrdiff_t;
    using value_type = NodeTy;
    using pointer = value_type*;
    using reference = value_type&;

    explicit IListIterator(pointer ptr) : m_ptr{ptr} {}

    NodeTy* getPtr() {
        return m_ptr;
    }

    // Accessors
    reference operator*() const { return *m_ptr; }
    pointer operator->() const noexcept {
        return m_ptr;
    }
    IListIterator getNext() {
        return IListIterator(static_cast<NodeTy*>(m_ptr->getNext()));
    }

    // Comparison operators
    bool operator==(const IListIterator& rhs) const noexcept {
      return m_ptr == rhs.m_ptr;
    }

    bool operator!=(const IListIterator& rhs) const noexcept {
      return this->m_ptr != rhs.m_ptr;
    }

    // Increment and decrement operators
    IListIterator& operator++() {
        m_ptr = static_cast<pointer>(m_ptr->getNext());
        return *this;
    }

    IListIterator& operator--() {
        m_ptr = static_cast<pointer>(m_ptr->getPrev());
        return *this;
    }

    IListIterator operator++(int) {
        auto tmp = *this;
        operator++();
        return tmp;
    }

    IListIterator operator--(int) {
        auto tmp = *this;
        operator--();
        return tmp;
    }

private:
    pointer m_ptr{nullptr};
};

template<typename NodeTy, bool Owner>
struct IListAllocTraits {};

template<typename NodeTy>
struct IListAllocTraits<NodeTy, true> {
    static void deallocate(NodeTy* node) { delete node; }
};

template<typename NodeTy>
using IListOwner = IListAllocTraits<NodeTy, true>;

template<typename NodeTy>
struct IListAllocTraits<NodeTy, false> {
    static void deallocate(NodeTy* node) {}
};

template<typename NodeTy>
using IListBorrower = IListAllocTraits<NodeTy, false>;

// TODO: IList parametrized with iterator to implement traversal algorithms on
// general interface
template<typename NodeTy>
struct IListDefaultTraits : IListOwner<NodeTy> {};

template<typename NodeTy, typename Traits = IListDefaultTraits<NodeTy>>
class IList : public IListBase {

public:
    using iterator = IListIterator<NodeTy>;
    using value_type = typename iterator::value_type;
    using pointer = typename iterator::pointer;

public:
    ~IList() {
        iterator it = begin();
        while (it != end()) {
            iterator next = it.getNext();
            Traits::deallocate(it.getPtr());
            it = next;
        }
    }

    iterator begin() const {
        return iterator{m_start};
    }

    iterator end() const {
        return iterator{nullptr};
    }

    void push_back(pointer node) {
        insertBefore(end(), node);
    }

    // TODO: add proxy elem at the start to avoid boilerplate checks
    iterator insertBefore(iterator I, pointer node) {
        if ((I == begin()) && (I == end())) {
            m_start = m_last = node;
            return iterator{node};
        }

        if (I == begin()) {
            m_start = node;
        }

        if (I == end()) {
            insertAfterBase(m_last, node);
            m_last = node;
            return iterator{node};
        }

        insertBeforeBase(I.getPtr(), node);
        return iterator{node};
    }

private:
    pointer m_last{nullptr};
    pointer m_start{nullptr};
};

} // namespace jade
