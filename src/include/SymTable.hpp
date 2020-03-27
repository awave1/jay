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
  int current_scope_level = 0;

  // aka position on the scope stack
  int current_scope = -1;

  SymTable();

  bool insert(Symbol symbol, scope_t scope);

  void push_scope(std::string fun_name);

  scope_t get_scope();

  void enter_scope();

  void exit_scope();

  void define(Symbol *symbol, std::string fun_name);

  Symbol *lookup(std::string name, std::string fun);

  bool exists(std::string name);

  FunctionSymbol *find_function(std::string name);

  friend std::ostream &operator<<(std::ostream &os, const SymTable &sym_table);

private:
  const int PREDEFINED_SCOPE = 0;
  const int GLOBAL_SCOPE = 1;

  const std::string PREDEFINED_SCOPE_NAME = "predefined";
  const std::string GLOBAL_SCOPE_NAME = "global";

  std::map<std::string, symbol_table_t> scope_stack;

  void add_predefined_symbols();
};

#endif /* SYM_TABLE_HPP */
