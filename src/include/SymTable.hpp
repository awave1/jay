#ifndef SYM_TABLE_HPP
#define SYM_TABLE_HPP

#include "FunctionSymbol.hpp"
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

typedef int scope_t;
typedef std::map<std::string, Symbol *> symbol_table_t;

class SymTable {
public:
  // scope nesting level
  scope_t current_scope = PREDEFINED_SCOPE;

  SymTable();

  bool insert(Symbol symbol, scope_t scope);

  void push_scope();

  scope_t get_scope();

  void enter_scope();

  void exit_scope();

  void define(Symbol *symbol);

  Symbol lookup(std::string name);

  bool has(std::string name);

  friend std::ostream &operator<<(std::ostream &os, const SymTable &sym_table);

private:
  const scope_t PREDEFINED_SCOPE = 0;
  const scope_t GLOBAL_SCOPE = 1;

  // represent symbol table as a scope stack
  // map stored: <key: symbol name, val: symbol>
  // position of the map
  std::vector<symbol_table_t> scope_stack;

  void add_predefined_symbols();
};

#endif /* SYM_TABLE_HPP */
