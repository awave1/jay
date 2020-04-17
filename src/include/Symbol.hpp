#ifndef SYMBOL_HPP
#define SYMBOL_HPP

#include "ASTNode.hpp"
#include <iostream>
#include <string>

using namespace yy;

class Symbol {
public:
  std::string name;
  std::string kind;
  std::string wasm_name;
  Node type;
  // what level is the symbol located
  int scope_level;
  // where on the scope stack it is
  int block_scope;

  Symbol(std::string name, std::string kind, Node type, int scope_level,
         int block_scope)
      : name(name), kind(kind), type(type), scope_level(scope_level),
        block_scope(block_scope) {
    wasm_name = "$" + name;
  }

  bool is_global() const { return scope_level == 1; }

  virtual void print(std::ostream &os) const {
    os << "<" << kind << ": " << get_str_for_type(type) << ", " << name
       << ", scope level: " << scope_level
       << ", block_scope (in symtable): " << block_scope
       << ", wasm_name: " << wasm_name << ">";
  }

  friend std::ostream &operator<<(std::ostream &os, const Symbol &s) {
    s.print(os);
    return os;
  }
};

#endif /* SYMBOL_HPP */
