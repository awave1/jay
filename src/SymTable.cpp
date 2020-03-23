#include "./include/SymTable.hpp"

SymTable::SymTable() {
  add_predefined_symbols();

  // insert empty global scope
  this->scope_stack.push_back({});
}

bool SymTable::insert(Symbol symbol, scope_t scope) {
  auto *symtable = &scope_stack[scope];

  symtable->insert(std::pair<Symbol, scope_t>(symbol, scope));

  return false;
}

/**
 * @brief pre-populate based on the predefined symbols
 */
void SymTable::add_predefined_symbols() {
  auto getchar_fun_sym = Symbol("getchar", ast_node_t::Node::int_t);
  auto halt_fun_sym = Symbol("halt", ast_node_t::Node::void_t);
  auto printb_fun_sym = Symbol("printb", ast_node_t::Node::void_t);
  auto printc_fun_sym = Symbol("printc", ast_node_t::Node::void_t);
  auto printi_fun_sym = Symbol("printi", ast_node_t::Node::void_t);
  auto prints_fun_sym = Symbol("prints", ast_node_t::Node::void_t);

  std::map<Symbol, scope_t> predefined_scope;

  predefined_scope.insert(
      std::pair<Symbol, scope_t>(getchar_fun_sym, PREDEFINED_SCOPE));
  predefined_scope.insert(
      std::pair<Symbol, scope_t>(halt_fun_sym, PREDEFINED_SCOPE));
  predefined_scope.insert(
      std::pair<Symbol, scope_t>(printb_fun_sym, PREDEFINED_SCOPE));
  predefined_scope.insert(
      std::pair<Symbol, scope_t>(printc_fun_sym, PREDEFINED_SCOPE));
  predefined_scope.insert(
      std::pair<Symbol, scope_t>(printi_fun_sym, PREDEFINED_SCOPE));
  predefined_scope.insert(
      std::pair<Symbol, scope_t>(prints_fun_sym, PREDEFINED_SCOPE));

  this->scope_stack.push_back(predefined_scope);
}

std::ostream &operator<<(std::ostream &os, const SymTable &sym_table) {
  for (scope_t i = 0; i < sym_table.scope_stack.size(); i++) {
    auto symtable = sym_table.scope_stack[i];
    os << "scope: " << i << std::endl;
    os << "symbol" << std::setw(20) << "scope" << std::endl;
    os << "----------------------------------" << std::endl;
    for (auto const &[symbol, scope] : symtable) {
      os << symbol << std::setw(10) << scope << std::endl;
    }
    os << "----------------------------------" << std::endl;
  }
  return os;
}