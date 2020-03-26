#ifndef SYMBOL_HPP
#define SYMBOL_HPP

#include "ast.hpp"
#include <iostream>
#include <string>

using namespace yy;

class Symbol {
public:
  std::string name;
  std::string kind;
  ast_node_t::Node type;
  std::size_t scope;

  Symbol(std::string name, std::string kind, ast_node_t::Node type,
         std::size_t scope)
      : name(name), kind(kind), type(type), scope(scope) {}

  virtual void print(std::ostream &os) const {
    os << "<" << kind << ": " << get_str_for_type(type) << ", " << name << ">";
  }

  friend std::ostream &operator<<(std::ostream &os, const Symbol &s) {
    s.print(os);
    return os;
  }
};

#endif /* SYMBOL_HPP */
