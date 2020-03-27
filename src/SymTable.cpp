#include "./include/SymTable.hpp"

SymTable::SymTable() {
  add_predefined_symbols();
  push_scope(GLOBAL_SCOPE_NAME);
  enter_scope();
}

void SymTable::define(Symbol *symbol, std::string fun_name) {
  if (scope_stack.find(fun_name) != scope_stack.end()) {
    auto *symtable = &scope_stack.at(fun_name);
    symtable->insert(std::pair<std::string, Symbol *>(symbol->name, symbol));
  } else {
    std::throw_with_nested(std::runtime_error("make sure scope for `" +
                                              fun_name + "` is created"));
  }
}

FunctionSymbol *SymTable::find_function(std::string name) {
  auto symtable = scope_stack.find(GLOBAL_SCOPE_NAME)->second;
  if (symtable.find(name) != symtable.end()) {
    return dynamic_cast<FunctionSymbol *>(symtable.at(name));
  } else {
    return nullptr;
  }
}

/**
 * a symbol can be defined either:
 *   - predefined (0)
 *   - globally (1)
 *   - locally (2...n), where n is scope for a function on the scope stack
 * first, lookup in the local scope
 * then check if it exists in global or predefined scope
 * if not found, return nullptr
 *
 * @param name
 * @param fun
 * @return Symbol*
 */
Symbol *SymTable::lookup(std::string name, std::string fun) {
  if (scope_stack.find(fun) != scope_stack.end()) {
    auto local_table = scope_stack.at(fun);
    auto it = local_table.find(name);
    if (it != local_table.end()) {
      return it->second;
    }
  }

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

  return nullptr;
}

/**
 * @brief pre-populate based on the predefined symbols
 */
void SymTable::add_predefined_symbols() {
  using namespace std;

  // built-in functions
  auto *getchar_fun_sym = new FunctionSymbol(
      "getchar", {}, ast_node_t::Node::int_t, PREDEFINED_SCOPE, 0);
  auto *halt_fun_sym = new FunctionSymbol("halt", {}, ast_node_t::Node::void_t,
                                          PREDEFINED_SCOPE, 0);
  auto *printb_fun_sym = new FunctionSymbol(
      "printb", {}, ast_node_t::Node::void_t, PREDEFINED_SCOPE, 0);
  auto *printc_fun_sym = new FunctionSymbol(
      "printc", {}, ast_node_t::Node::void_t, PREDEFINED_SCOPE, 0);
  auto *printi_fun_sym = new FunctionSymbol(
      "printi", {}, ast_node_t::Node::void_t, PREDEFINED_SCOPE, 0);
  auto *prints_fun_sym = new FunctionSymbol(
      "prints", {}, ast_node_t::Node::void_t, PREDEFINED_SCOPE, 0);

  push_scope(PREDEFINED_SCOPE_NAME);

  define(getchar_fun_sym, PREDEFINED_SCOPE_NAME);
  define(halt_fun_sym, PREDEFINED_SCOPE_NAME);
  define(printb_fun_sym, PREDEFINED_SCOPE_NAME);
  define(printc_fun_sym, PREDEFINED_SCOPE_NAME);
  define(printi_fun_sym, PREDEFINED_SCOPE_NAME);
  define(prints_fun_sym, PREDEFINED_SCOPE_NAME);
}

void SymTable::push_scope(std::string fun_name) {
  if (!fun_name.empty()) {
    scope_stack.insert(std::pair<std::string, symbol_table_t>(fun_name, {}));
  }
  current_scope++;
}

void SymTable::exit_scope() {
  current_scope_level--;
  // std::cout << "exit scope: current scope " << current_scope_level <<
  // std::endl;
}

void SymTable::enter_scope() {
  current_scope_level++;
  // std::cout << "enter scope: current scope " << current_scope_level <<
  // std::endl;
}

scope_t SymTable::get_scope() { return current_scope_level; }

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