#ifndef FUNCTION_SYMBOL_HPP
#define FUNCTION_SYMBOL_HPP

#include "Symbol.hpp"
#include "ast.hpp"
#include <vector>

using namespace yy;

class FunctionSymbol : public Symbol {
public:
  FunctionSymbol(std::string name, std::vector<Symbol> params,
                 ast_node_t::Node type)
      : Symbol(name, "function", type), params(params){};

  std::vector<Symbol> params;

  void print(std::ostream &os) const {
    os << "<" << kind << ": " << get_str_for_type(type) << ", " << name
       << ", params: ";
    for (auto p : params) {
      os << p << " ";
    }
    os << ">";
  }
};

#endif /* FUNCTION_SYMBOL_HPP */