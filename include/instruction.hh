#pragma once

#include <memory>
#include <vector>

#include "value.hh"
#include "ilist.hh"

namespace jade {

class BasicBlock;

class Instruction : public Value, public IListNode<Instruction> {
public:

  template<typename T, typename... Args>
  static std::unique_ptr<T> Create(Args&&... args);

private:
  // Values m_inputs;
};

class IfInstr : public Instruction {
public:

private:
};

class BinaryInstr;

template <typename T, typename... Args>
std::unique_ptr<T> Instruction::Create(Args&&... args) {
  return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}

} // namespace jade
