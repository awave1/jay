#ifndef SYM_TABLE_HPP
#define SYM_TABLE_HPP

#include <algorithm>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

class SymTable {
public:
  SymTable();

  bool has(std::string symbol_name);

  int get(std::string symbol_name);

  bool insert(std::string symbol_name, int scope);

  bool set(std::string symbol_name, int scope);

  void push_scope();

  std::unordered_map<std::string, int> pop_scope();

private:
  // map name (?) to scope level(?)
  std::vector<std::unordered_map<std::string, int>> scope_stack;
};

#endif /* SYM_TABLE_HPP */
