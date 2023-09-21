#pragma once

#include "ilist.hh"
#include <memory>

namespace jade {

class Type {
public:
  enum Tag {
    I8 = 0,
    I16,
    I32,
    I64,
    I1, // bool
    None,
  };

  Type(Tag tag) : m_tag(tag) {}

private:
  Tag m_tag;
};

class Value {
  Type m_type{Type::None};
};

struct IValue : public Value, public IListNode<Value> {};
using Values = std::unique_ptr<IValue>;

} // namespace jade
