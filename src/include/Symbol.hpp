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

  Symbol(std::string name, std::string kind, ast_node_t::Node type)
      : name(name), kind(kind), type(type) {}

  friend std::ostream &operator<<(std::ostream &os, const Symbol &s) {
    os << "<" << s.kind << ": " << get_str_for_type(s.type) << ", " << s.name
       << ">";
    return os;
  }
};

#endif /* SYMBOL_HPP */
