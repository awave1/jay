#ifndef SEMANTIC_ANALYZER_H
#define SEMANTIC_ANALYZER_H

#include "FunctionSymbol.hpp"
#include "SymTable.hpp"
#include "Symbol.hpp"
#include "ast.hpp"
#include <iostream>
#include <map>
#include <memory>
#include <stdexcept>
#include <vector>

using namespace yy;

struct bool_expr_record {
  ast_node_t::Node l;
  ast_node_t::Node r;
  ast_node_t::Node result;
};

typedef std::vector<std::vector<ast_node_t::Node>> expr_list_t;
// typedef std::vector<bool_expr_record> expr_list_t;

class SemanticAnalyzer {
public:
  SemanticAnalyzer(std::shared_ptr<ast_node_t> ast,
                   std::shared_ptr<SymTable> sym_table)
      : ast(ast), sym_table(sym_table) {
    expression_types.insert(std::pair<ast_node_t::Node, expr_list_t>(
        ast_node_t::Node::add_op,
        {{ast_node_t::Node::int_t, ast_node_t::Node::int_t,
          ast_node_t::Node::int_t}}));
    expression_types.insert(std::pair<ast_node_t::Node, expr_list_t>(
        ast_node_t::Node::sub_op,
        {{ast_node_t::Node::int_t, ast_node_t::Node::int_t,
          ast_node_t::Node::int_t}}));
    expression_types.insert(std::pair<ast_node_t::Node, expr_list_t>(
        ast_node_t::Node::div_op,
        {{ast_node_t::Node::int_t, ast_node_t::Node::int_t,
          ast_node_t::Node::int_t}}));
    expression_types.insert(std::pair<ast_node_t::Node, expr_list_t>(
        ast_node_t::Node::mod_op,
        {{ast_node_t::Node::int_t, ast_node_t::Node::int_t,
          ast_node_t::Node::int_t}}));
    expression_types.insert(std::pair<ast_node_t::Node, expr_list_t>(
        ast_node_t::Node::mul_op,
        {{ast_node_t::Node::int_t, ast_node_t::Node::int_t,
          ast_node_t::Node::int_t}}));
    expression_types.insert(std::pair<ast_node_t::Node, expr_list_t>(
        ast_node_t::Node::lt_op,
        {{ast_node_t::Node::int_t, ast_node_t::Node::int_t,
          ast_node_t::Node::boolean_t}}));
    expression_types.insert(std::pair<ast_node_t::Node, expr_list_t>(
        ast_node_t::Node::gt_op,
        {{ast_node_t::Node::int_t, ast_node_t::Node::int_t,
          ast_node_t::Node::boolean_t}}));
    expression_types.insert(std::pair<ast_node_t::Node, expr_list_t>(
        ast_node_t::Node::lteq_op,
        {{ast_node_t::Node::int_t, ast_node_t::Node::int_t,
          ast_node_t::Node::boolean_t}}));
    expression_types.insert(std::pair<ast_node_t::Node, expr_list_t>(
        ast_node_t::Node::gteq_op,
        {{ast_node_t::Node::int_t, ast_node_t::Node::int_t,
          ast_node_t::Node::boolean_t}}));
    expression_types.insert(std::pair<ast_node_t::Node, expr_list_t>(
        ast_node_t::Node::eqeq_op,
        {{ast_node_t::Node::int_t, ast_node_t::Node::int_t,
          ast_node_t::Node::int_t},
         {ast_node_t::Node::boolean_t, ast_node_t::Node::boolean_t,
          ast_node_t::Node::boolean_t}}));
    expression_types.insert(std::pair<ast_node_t::Node, expr_list_t>(
        ast_node_t::Node::noteq_op,
        {{ast_node_t::Node::int_t, ast_node_t::Node::int_t,
          ast_node_t::Node::boolean_t},
         {ast_node_t::Node::boolean_t, ast_node_t::Node::boolean_t,
          ast_node_t::Node::boolean_t}}));
    expression_types.insert(std::pair<ast_node_t::Node, expr_list_t>(
        ast_node_t::Node::not_op,
        {{ast_node_t::Node::boolean_t, ast_node_t::Node::boolean_t}}));
    expression_types.insert(std::pair<ast_node_t::Node, expr_list_t>(
        ast_node_t::Node::bin_and_op,
        {{ast_node_t::Node::boolean_t, ast_node_t::Node::boolean_t,
          ast_node_t::Node::boolean_t}}));
    expression_types.insert(std::pair<ast_node_t::Node, expr_list_t>(
        ast_node_t::Node::bin_or_op,
        {{ast_node_t::Node::boolean_t, ast_node_t::Node::boolean_t,
          ast_node_t::Node::boolean_t}}));
  }
  ~SemanticAnalyzer() {}

  std::shared_ptr<ast_node_t> get_ast();

  bool validate();

  bool is_declaration_allowed();

private:
  std::shared_ptr<ast_node_t> ast;
  std::shared_ptr<SymTable> sym_table;
  std::map<ast_node_t::Node, expr_list_t> expression_types;

  bool traverse(ast_node_t *node, std::function<bool(ast_node_t *n)> pre,
                std::function<void(ast_node_t *n)> post);

  // pass 1
  void globals_post_order_pass(ast_node_t *node);

  // pass 2
  void sym_table_pre_post_order_pass(ast_node_t *node);

  // pass 3
  void type_checking_post_order_pass(ast_node_t *node);

  // pass 4
  bool catch_all_pre_post_order_pass(ast_node_t *node);

  bool build_scope(ast_node_t *node);
};

#endif /* SEMANTIC_ANALYZER_H */