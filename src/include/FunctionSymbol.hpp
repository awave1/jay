#ifndef FUNCTION_SYMBOL_HPP
#define FUNCTION_SYMBOL_HPP

#include "ASTNode.hpp"
#include "Symbol.hpp"
#include <vector>

using namespace yy;

class FunctionSymbol : public Symbol {
public:
  FunctionSymbol(std::string name, std::vector<Symbol> params, Node type,
                 int scope_level, int block_scope)
      : Symbol(name, "function", type, scope_level, block_scope),
        params(params){};

  std::vector<Symbol> params;

  void print(std::ostream &os) const {
    os << "<" << kind << ": " << get_str_for_type(type) << ", " << name
       << ", params: ";
    for (auto p : params) {
      os << p << " ";
    }
    os << ", scope: " << scope_level << ">"
       << ", block_scope (in symtable): " << block_scope << ">";
    os << ">";
  }
};

#endif /* FUNCTION_SYMBOL_HPP */