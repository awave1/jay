#ifndef SYM_TABLE_HPP
#define SYM_TABLE_HPP

#include "Symbol.hpp"
#include "ast.hpp"
#include <algorithm>
#include <iomanip>
#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <vector>

using namespace yy;

typedef std::size_t scope_t;

class SymTable {
public:
  SymTable();

  // bool has(std::string symbol_name);

  // int get(std::string symbol_name);

  bool insert(Symbol symbol, scope_t scope);

  // bool set(Symbol symbol, int scope);

  // void push_scope();

  // std::map<Symbol, int> pop_scope();

  friend std::ostream &operator<<(std::ostream &os, const SymTable &sym_table);

private:
  const scope_t PREDEFINED_SCOPE = 0;
  const scope_t GLOBAL_SCOPE = 1;

  // represent symbol table as a scope stack
  // map stored: <key: symbol, val: scope (position in the vector)>
  std::vector<std::map<Symbol, scope_t>> scope_stack;

  void add_predefined_symbols();
};

#endif /* SYM_TABLE_HPP */
