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

typedef std::vector<std::vector<ast_node_t::Node>> expr_list_t;

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
          ast_node_t::Node::int_t},
         {ast_node_t::Node::int_t, ast_node_t::Node::int_t}}));
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

  void traverse(
      ast_node_t *node,
      std::function<void(ast_node_t *n, std::vector<bool> &err_stack)> pre,
      std::function<void(ast_node_t *n, std::vector<bool> &err_stack)> post,
      std::vector<bool> &err_stack);

  // pass 1
  void globals_post_order_pass(ast_node_t *node, std::vector<bool> &err_stack);

  // pass 2
  void sym_table_post_pass(ast_node_t *node, std::vector<bool> &err_stack);

  // pass 3
  void type_checking_post_order_pass(ast_node_t *node,
                                     std::vector<bool> &err_stack);

  void enter_scope(ast_node_t *node, std::vector<bool> &err_stack);

  void build_scope(ast_node_t *node, std::vector<bool> &err_stack);

  bool validate_expr(ast_node_t *expr, expr_list_t expected_tyes);

  void semantic_error(std::string msg, int linenum);
  void semantic_error(std::string msg);
};

#endif /* SEMANTIC_ANALYZER_H */