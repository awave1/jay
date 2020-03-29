/**
 * @file SymTable.cpp
 * @author Artem Golovin (30018900)
 * @brief Implementation of Symbol table using variation of scope stack. Symbol
 * table stack is built using hash map with scope name as a key (either function
 * name or hardcoded globad/predefined) and symbol table hash map as its value.
 */

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

typedef std::map<std::string, Symbol *> symbol_table_t;

/**
 * @brief Scope stack symbol table implementation
 */
class SymTable {
public:
  // scope nesting level
  int current_scope_level = 0;

  // aka position on the scope stack
  int current_scope = -1;

  SymTable();

  /**
   * @brief insert a new scope for a function of specified name
   *
   * @param fun_name name of the function to define a scope
   */
  void push_scope(std::string fun_name);

  /**
   * @brief increment current block nesting level
   */
  void enter_scope();

  /**
   * @brief decrement current block nesting level
   */
  void exit_scope();

  /**
   * @brief define a symbol in specified function
   *
   * @param symbol symbol to define
   * @param fun_name name of the function (scope for the symbol)
   */
  void define(Symbol *symbol, std::string fun_name);

  /**
   * @brief lookup a symbol of a given name inside specified function scope. if
   * a symbol doesn't exist in the specified function scope look in global and
   * predefined scopes before returning nullptr.
   *
   * @param name symbol name to look for
   * @param fun scope of the symbol
   * @return Symbol* resulting symbol
   */
  Symbol *lookup(std::string name, std::string fun);

  /**
   * @brief lookup a symbol of a given name only inside specified function
   * scope.
   *
   * @param name symbol name to look for
   * @param fun scope of the symbol
   * @return Symbol* resulting symbol
   */
  Symbol *lookup_in_local(std::string name, std::string fun);

  /**
   * @brief find a function with a given name
   *
   * @param name function name
   * @return FunctionSymbol*  resulting function symbol
   */
  FunctionSymbol *find_function(std::string name);

  friend std::ostream &operator<<(std::ostream &os, const SymTable &sym_table);

private:
  const int PREDEFINED_SCOPE = 0;
  const int GLOBAL_SCOPE = 1;

  const std::string PREDEFINED_SCOPE_NAME = "predefined";
  const std::string GLOBAL_SCOPE_NAME = "global";

  std::map<std::string, symbol_table_t> scope_stack;

  /**
   * @brief build any predefined symbols (functions, variables)
   */
  void add_predefined_symbols();
};

#endif /* SYM_TABLE_HPP */
