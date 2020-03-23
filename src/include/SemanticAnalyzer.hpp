#ifndef SEMANTIC_ANALYZER_H
#define SEMANTIC_ANALYZER_H

#include "SymTable.hpp"
#include "ast.hpp"
#include <iostream>
#include <memory>

using namespace yy;

class SemanticAnalyzer {
public:
  SemanticAnalyzer(std::shared_ptr<ast_node_t> ast,
                   std::shared_ptr<SymTable> sym_table)
      : ast(ast), sym_table(sym_table) {}
  ~SemanticAnalyzer() {}

  std::shared_ptr<ast_node_t> get_ast();

  bool validate();

private:
  std::shared_ptr<ast_node_t> ast;
  std::shared_ptr<SymTable> sym_table;

  void traverse(ast_node_t *node, std::function<bool(ast_node_t *n)> pre,
                std::function<bool(ast_node_t *n)> post);

  // pass 1
  bool globals_post_order_pass(ast_node_t *node);

  // pass 2
  bool sym_table_pre_post_order_pass(ast_node_t *node);
  // pass 3
  bool type_checking_post_order_pass(ast_node_t *node);

  // pass 4
  bool catch_all_pre_post_order_pass(ast_node_t *node);
};

#endif /* SEMANTIC_ANALYZER_H */