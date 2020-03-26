#include "./include/SymTable.hpp"

SymTable::SymTable() {
  add_predefined_symbols();
  push_scope();
  enter_scope();
}

void SymTable::define(Symbol *symbol) {
  auto *symtable = &scope_stack.back();
  symtable->insert(std::pair<std::string, Symbol *>(symbol->name, symbol));
}

bool SymTable::has(std::string name) {
  auto symtable = scope_stack.back();
  auto iter = symtable.find(name);
  return iter != symtable.end();
}

bool SymTable::exists(std::string name) {
  for (auto i = scope_stack.rbegin(); i != scope_stack.rend(); i++) {
    auto table = *i;
    if (table.find(name) != table.end()) {
      return true;
    }
  }

  return false;
}

FunctionSymbol *SymTable::find_function(std::string name) {
  auto symtable = scope_stack[GLOBAL_SCOPE];
  return dynamic_cast<FunctionSymbol *>(symtable.at(name));
}

/**
 * @brief pre-populate based on the predefined symbols
 */
void SymTable::add_predefined_symbols() {
  using namespace std;

  // built-in functions
  auto *getchar_fun_sym = new FunctionSymbol(
      "getchar", {}, ast_node_t::Node::int_t, PREDEFINED_SCOPE);
  auto *halt_fun_sym = new FunctionSymbol("halt", {}, ast_node_t::Node::void_t,
                                          PREDEFINED_SCOPE);
  auto *printb_fun_sym = new FunctionSymbol(
      "printb", {}, ast_node_t::Node::void_t, PREDEFINED_SCOPE);
  auto *printc_fun_sym = new FunctionSymbol(
      "printc", {}, ast_node_t::Node::void_t, PREDEFINED_SCOPE);
  auto *printi_fun_sym = new FunctionSymbol(
      "printi", {}, ast_node_t::Node::void_t, PREDEFINED_SCOPE);
  auto *prints_fun_sym = new FunctionSymbol(
      "prints", {}, ast_node_t::Node::void_t, PREDEFINED_SCOPE);

  push_scope();

  define(getchar_fun_sym);
  define(halt_fun_sym);
  define(printb_fun_sym);
  define(printc_fun_sym);
  define(printi_fun_sym);
  define(prints_fun_sym);
}

void SymTable::push_scope() { scope_stack.push_back({}); }

void SymTable::exit_scope() {
  current_scope--;
  // std::cout << "exit scope: current scope " << current_scope << std::endl;
}

void SymTable::enter_scope() {
  current_scope++;
  // std::cout << "enter scope: current scope " << current_scope << std::endl;
}

scope_t SymTable::get_scope() { return current_scope; }

std::ostream &operator<<(std::ostream &os, const SymTable &sym_table) {
  for (scope_t i = 0; i < sym_table.scope_stack.size(); i++) {
    auto symtable = sym_table.scope_stack[i];
    os << "scope: " << i << std::endl;
    os << "----------------------------------" << std::endl;
    for (auto const &[name, symbol] : symtable) {
      os << name << std::setw(10) << *symbol << std::endl;
    }
    os << "----------------------------------" << std::endl;
  }
  return os;
}