#include "./include/SymTable.hpp"

SymTable::SymTable() { push_scope(); }

bool SymTable::has(std::string symbol_name) {
  // TODO: refactor
  for (auto scope : scope_stack) {
    if (scope.find(symbol_name) != scope.end()) {
      return true;
    }
  }

  return false;
}

int SymTable::get(std::string symbol_name) {
  // TODO: refactor
  for (auto scope : scope_stack) {
    if (scope.find(symbol_name) != scope.end()) {
      return scope.at(symbol_name);
    }
  }
  // TODO: error
}

bool SymTable::insert(std::string symbol_name, int scope) {
  auto &current = scope_stack.back();
  bool is_inserted = current.try_emplace(symbol_name, scope).second;
  return is_inserted;
}

bool SymTable::set(std::string symbol_name, int scope) {
  for (auto scope : scope_stack) {
    auto iter = scope.find(symbol_name);
    if (iter != scope.end()) {
    }
  }
}

void SymTable::push_scope() {
  // push empty scope ??
  this->scope_stack.push_back({});
}

std::unordered_map<std::string, int> SymTable::pop_scope() {
  auto last = scope_stack.back();
  scope_stack.pop_back();
  return last;
}