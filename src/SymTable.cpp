/**
 * @file SymTable.cpp
 * @author Artem Golovin (30018900)
 * @brief Implementation of Symbol table using variation of scope stack. Symbol
 * table stack is built using hash map with scope name as a key (either function
 * name or hardcoded globad/predefined) and symbol table hash map as its value.
 */
#include "./include/SymTable.hpp"

SymTable::SymTable() {
  add_predefined_symbols();
  push_scope(GLOBAL_SCOPE_NAME);
  enter_scope();
}

/**
 * @brief define a symbol in specified function
 *
 * @param symbol symbol to define
 * @param fun_name name of the function (scope for the symbol)
 */
void SymTable::define(Symbol *symbol, std::string fun_name) {
  if (scope_stack.find(fun_name) != scope_stack.end()) {
    auto *symtable = &scope_stack.at(fun_name);
    symtable->insert(std::pair<std::string, Symbol *>(symbol->name, symbol));
  } else {
    std::throw_with_nested(std::runtime_error("make sure scope for `" +
                                              fun_name + "` is created"));
  }
}

/**
 * @brief find a function with a given name
 *
 * @param name function name
 * @return FunctionSymbol*  resulting function symbol
 */
FunctionSymbol *SymTable::find_function(std::string name) {
  auto glob_symtable = scope_stack.find(GLOBAL_SCOPE_NAME)->second;
  if (glob_symtable.find(name) != glob_symtable.end()) {
    try {
      return dynamic_cast<FunctionSymbol *>(glob_symtable.at(name));
    } catch (const std::exception &err) {
      std::cerr << "failed to lookup function `" << name << "`: " << err.what()
                << std::endl;
      return nullptr;
    }
  }

  auto predefined_symtable = scope_stack.find(PREDEFINED_SCOPE_NAME)->second;
  if (predefined_symtable.find(name) != predefined_symtable.end()) {
    try {
      return dynamic_cast<FunctionSymbol *>(predefined_symtable.at(name));
    } catch (const std::exception &err) {
      std::cerr << "failed to lookup function `" << name << "`: " << err.what()
                << std::endl;
      return nullptr;
    }
  }

  return nullptr;
}

/**
 * @brief lookup a symbol of a given name inside specified function scope. if
 * a symbol doesn't exist in the specified function scope look in global and
 * predefined scopes before returning nullptr.
 *
 * @param name symbol name to look for
 * @param fun scope of the symbol
 * @return Symbol* resulting symbol
 */
Symbol *SymTable::lookup(std::string name, std::string fun) {
  if (scope_stack.find(fun) != scope_stack.end()) {
    auto local_table = scope_stack.at(fun);
    auto it = local_table.find(name);
    if (it != local_table.end()) {
      return it->second;
    }
  }

  try {
    auto global_table = scope_stack.at(GLOBAL_SCOPE_NAME);

    auto g_find = global_table.find(name);
    if (g_find != global_table.end()) {
      return g_find->second;
    }

    auto predefined_table = scope_stack.at(PREDEFINED_SCOPE_NAME);
    auto p_find = predefined_table.find(name);
    if (p_find != predefined_table.end()) {
      return p_find->second;
    }
  } catch (const std::exception &err) {
    std::cerr << "failed to lookup`" << name << "` at `" << fun
              << "`: " << err.what() << std::endl;
    return nullptr;
  }

  return nullptr;
}

/**
 * @brief lookup a symbol of a given name only inside specified function
 * scope.
 *
 * @param name symbol name to look for
 * @param fun scope of the symbol
 * @return Symbol* resulting symbol
 */
Symbol *SymTable::lookup_in_local(std::string name, std::string fun) {
  if (scope_stack.find(fun) != scope_stack.end()) {
    auto local_table = scope_stack.at(fun);
    auto it = local_table.find(name);
    if (it != local_table.end()) {
      return it->second;
    }
  }

  return nullptr;
}

/**
 * @brief pre-populate based on the predefined symbols
 */
void SymTable::add_predefined_symbols() {
  using namespace std;

  // built-in functions
  auto *getchar_fun_sym =
      new FunctionSymbol("getchar", {}, Node::int_t, PREDEFINED_SCOPE, 0);
  auto *halt_fun_sym =
      new FunctionSymbol("halt", {}, Node::void_t, PREDEFINED_SCOPE, 0);
  auto *printb_fun_sym = new FunctionSymbol(
      "printb", {Symbol("b", "parameter", Node::boolean_t, 0, 0)}, Node::void_t,
      PREDEFINED_SCOPE, 0);
  auto *printc_fun_sym = new FunctionSymbol(
      "printc", {Symbol("c", "parameter", Node::int_t, 0, 0)}, Node::void_t,
      PREDEFINED_SCOPE, 0);
  auto *printi_fun_sym = new FunctionSymbol(
      "printi", {Symbol("i", "parameter", Node::int_t, 0, 0)}, Node::void_t,
      PREDEFINED_SCOPE, 0);
  auto *prints_fun_sym = new FunctionSymbol(
      "prints", {Symbol("s", "parameter", Node::string, 0, 0)}, Node::void_t,
      PREDEFINED_SCOPE, 0);

  // boolean operation functions
  auto *and_op_fun_sym =
      new FunctionSymbol("__and_op",
                         {Symbol("lhs", "parameter", Node::boolean_t, 0, 0),
                          Symbol("rhs", "parameter", Node::boolean_t, 0, 0)},
                         Node::boolean_t, PREDEFINED_SCOPE, 0);
  auto *or_op_fun_sym =
      new FunctionSymbol("__or_op",
                         {Symbol("lhs", "parameter", Node::boolean_t, 0, 0),
                          Symbol("rhs", "parameter", Node::boolean_t, 0, 0)},
                         Node::boolean_t, PREDEFINED_SCOPE, 0);

  push_scope(PREDEFINED_SCOPE_NAME);

  define(getchar_fun_sym, PREDEFINED_SCOPE_NAME);
  define(halt_fun_sym, PREDEFINED_SCOPE_NAME);
  define(printb_fun_sym, PREDEFINED_SCOPE_NAME);
  define(printc_fun_sym, PREDEFINED_SCOPE_NAME);
  define(printi_fun_sym, PREDEFINED_SCOPE_NAME);
  define(prints_fun_sym, PREDEFINED_SCOPE_NAME);
  define(and_op_fun_sym, PREDEFINED_SCOPE_NAME);
  define(or_op_fun_sym, PREDEFINED_SCOPE_NAME);
}

/**
 * @brief insert a new scope for a function of specified name
 *
 * @param fun_name name of the function to define a scope
 */
void SymTable::push_scope(std::string fun_name) {
  if (!fun_name.empty()) {
    scope_stack.insert(std::pair<std::string, symbol_table_t>(fun_name, {}));
  }
  current_scope++;
}

symbol_table_t SymTable::get_scope(std::string scope_name) {
  return scope_stack.at(scope_name);
}

/**
 * @brief increment current block nesting level
 */
void SymTable::enter_scope() { current_scope_level++; }

/**
 * @brief decrement current block nesting level
 */
void SymTable::exit_scope() { current_scope_level--; }

std::ostream &operator<<(std::ostream &os, const SymTable &sym_table) {
  for (auto const &[fun, symtable] : sym_table.scope_stack) {
    os << "scope: " << fun << std::endl;
    os << "----------------------------------" << std::endl;
    for (auto const &[name, symbol] : symtable) {
      os << name << std::setw(10) << *symbol << std::endl;
    }
    os << "----------------------------------" << std::endl;
  }
  return os;
}