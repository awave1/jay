#ifndef SEMANTIC_ANALYZER_H
#define SEMANTIC_ANALYZER_H

#include "FunctionSymbol.hpp"
#include "SymTable.hpp"
#include "Symbol.hpp"
#include "ast.hpp"
#include <iostream>
#include <memory>
#include <stdexcept>

using namespace yy;

class SemanticAnalyzer {
public:
  SemanticAnalyzer(std::shared_ptr<ast_node_t> ast,
                   std::shared_ptr<SymTable> sym_table)
      : ast(ast), sym_table(sym_table) {}
  ~SemanticAnalyzer() {}

  std::shared_ptr<ast_node_t> get_ast();

  bool validate();

  bool is_declaration_allowed();

private:
  std::shared_ptr<ast_node_t> ast;
  std::shared_ptr<SymTable> sym_table;

  bool traverse(ast_node_t *node, std::function<bool(ast_node_t *n)> pre,
                std::function<void(ast_node_t *n)> post);

  // pass 1
  void globals_post_order_pass(ast_node_t *node);

  // pass 2
  void sym_table_pre_post_order_pass(ast_node_t *node);

  // pass 3
  bool type_checking_post_order_pass(ast_node_t *node);

  // pass 4
  bool catch_all_pre_post_order_pass(ast_node_t *node);

  bool build_scope(ast_node_t *node);
};

#endif /* SEMANTIC_ANALYZER_H */