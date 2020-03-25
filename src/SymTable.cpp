#include "./include/SymTable.hpp"

SymTable::SymTable() {
  add_predefined_symbols();
  push_scope();
}

void SymTable::define(Symbol symbol) {
  auto *symtable = &scope_stack.back();
  symtable->insert(std::pair<std::string, Symbol>(symbol.name, symbol));
}

bool SymTable::has(std::string name) {
  auto symtable = scope_stack.back();
  auto iter = symtable.find(name);
  return iter != symtable.end();
}

/**
 * @brief pre-populate based on the predefined symbols
 */
void SymTable::add_predefined_symbols() {
  using namespace std;

  // built-in functions
  auto getchar_fun_sym = FunctionSymbol("getchar", {}, ast_node_t::Node::int_t);
  auto halt_fun_sym = FunctionSymbol("halt", {}, ast_node_t::Node::void_t);
  auto printb_fun_sym = FunctionSymbol("printb", {}, ast_node_t::Node::void_t);
  auto printc_fun_sym = FunctionSymbol("printc", {}, ast_node_t::Node::void_t);
  auto printi_fun_sym = FunctionSymbol("printi", {}, ast_node_t::Node::void_t);
  auto prints_fun_sym = FunctionSymbol("prints", {}, ast_node_t::Node::void_t);

  push_scope();

  define(getchar_fun_sym);
  define(halt_fun_sym);
  define(printb_fun_sym);
  define(printc_fun_sym);
  define(printi_fun_sym);
  define(prints_fun_sym);
}

void SymTable::push_scope() { scope_stack.push_back({}); }

std::ostream &operator<<(std::ostream &os, const SymTable &sym_table) {
  for (scope_t i = 0; i < sym_table.scope_stack.size(); i++) {
    auto symtable = sym_table.scope_stack[i];
    os << "scope: " << i << std::endl;
    os << "----------------------------------" << std::endl;
    for (auto const &[symbol, scope] : symtable) {
      os << symbol << std::setw(10) << scope << std::endl;
    }
    os << "----------------------------------" << std::endl;
  }
  return os;
}