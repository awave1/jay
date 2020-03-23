#ifndef SYMBOL_HPP
#define SYMBOL_HPP

#include "ast.hpp"
#include <iostream>
#include <string>

using namespace yy;

class Symbol {
public:
  std::string name;
  ast_node_t::Node type;

  Symbol(std::string name, ast_node_t::Node type) : name(name), type(type) {}

  friend std::ostream &operator<<(std::ostream &os, const Symbol &s) {
    os << "<" << get_str_for_type(s.type) << ", " << s.name << ">";
    return os;
  }
};

// necessary to be able to use Symbol in a hashmap as a key
namespace std {
template <> struct less<Symbol> {
  bool operator()(const Symbol &lhs, const Symbol &rhs) const {
    return lhs.name < rhs.name;
  }
};
} // namespace std
#endif /* SYMBOL_HPP */
