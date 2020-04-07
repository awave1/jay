/**
 * @file SemanticAnalyzer.cpp
 * @author Artem Golovin (30018900)
 * @brief Perform semantic analysis of the parsed AST and build a symbol table
 */

#ifndef SEMANTIC_ANALYZER_H
#define SEMANTIC_ANALYZER_H

#include "ASTNode.hpp"
#include "FunctionSymbol.hpp"
#include "SymTable.hpp"
#include "Symbol.hpp"
#include <iostream>
#include <map>
#include <memory>
#include <stdexcept>
#include <vector>

using namespace yy;

typedef std::vector<std::vector<Node>> expr_list_t;

/**
 * @brief perform semantic analysis of the ast and build a symbol table
 */
class SemanticAnalyzer {
public:
  std::shared_ptr<SymTable> sym_table;

  SemanticAnalyzer(std::shared_ptr<ASTNode> ast) : ast(ast) {
    sym_table = std::shared_ptr<SymTable>(new SymTable());
    // build the list of expected types for bool and math expressions
    expression_types.insert(std::pair<Node, expr_list_t>(
        Node::add_op, {{Node::int_t, Node::int_t, Node::int_t}}));
    expression_types.insert(std::pair<Node, expr_list_t>(
        Node::sub_op,
        {{Node::int_t, Node::int_t, Node::int_t}, {Node::int_t, Node::int_t}}));
    expression_types.insert(std::pair<Node, expr_list_t>(
        Node::div_op, {{Node::int_t, Node::int_t, Node::int_t}}));
    expression_types.insert(std::pair<Node, expr_list_t>(
        Node::mod_op, {{Node::int_t, Node::int_t, Node::int_t}}));
    expression_types.insert(std::pair<Node, expr_list_t>(
        Node::mul_op, {{Node::int_t, Node::int_t, Node::int_t}}));
    expression_types.insert(std::pair<Node, expr_list_t>(
        Node::lt_op, {{Node::int_t, Node::int_t, Node::boolean_t}}));
    expression_types.insert(std::pair<Node, expr_list_t>(
        Node::gt_op, {{Node::int_t, Node::int_t, Node::boolean_t}}));
    expression_types.insert(std::pair<Node, expr_list_t>(
        Node::lteq_op, {{Node::int_t, Node::int_t, Node::boolean_t}}));
    expression_types.insert(std::pair<Node, expr_list_t>(
        Node::gteq_op, {{Node::int_t, Node::int_t, Node::boolean_t}}));
    expression_types.insert(std::pair<Node, expr_list_t>(
        Node::eqeq_op, {{Node::int_t, Node::int_t, Node::int_t},
                        {Node::boolean_t, Node::boolean_t, Node::boolean_t}}));
    expression_types.insert(std::pair<Node, expr_list_t>(
        Node::noteq_op, {{Node::int_t, Node::int_t, Node::boolean_t},
                         {Node::boolean_t, Node::boolean_t, Node::boolean_t}}));
    expression_types.insert(std::pair<Node, expr_list_t>(
        Node::not_op, {{Node::boolean_t, Node::boolean_t}}));
    expression_types.insert(std::pair<Node, expr_list_t>(
        Node::bin_and_op,
        {{Node::boolean_t, Node::boolean_t, Node::boolean_t}}));
    expression_types.insert(std::pair<Node, expr_list_t>(
        Node::bin_or_op,
        {{Node::boolean_t, Node::boolean_t, Node::boolean_t}}));
  }

  /**
   * @brief perform semantic validation of the ast and build a symtable. the
   * method traverses ast multiple times to find all semantic errors
   *
   * @return true if ast is semantically correct and passed all the checks
   * @return false otherwise
   */
  bool validate();

private:
  std::shared_ptr<ASTNode> ast;
  std::map<Node, expr_list_t> expression_types;

  /**
   * @brief check if declaration is allowed at current block level. the method
   * gets the current block level from the symbol table and verifies allowed
   * block leves. Allowed block levels can be global (1) or local (2). If it's
   * greater than 2, then return false
   *
   * @return true, if current block level allows to declare variables
   * @return false, if block is too deep, and therefore declaration is not
   * allowed
   */
  bool is_declaration_allowed();

  /**
   * @brief traverse the ast with possible pre and post order callbacks.
   *
   * @param node ast node to start traversing from
   * @param pre pre-order callback function
   * @param post pos-order callback function
   * @param err_stack a vector to keep track of errors displayed. Rather
   * **hacky** way to allow semantic analyzer to print all errors after checks
   * have been performed
   */
  void
  traverse(ASTNode *node,
           std::function<void(ASTNode *n, std::vector<bool> &err_stack)> pre,
           std::function<void(ASTNode *n, std::vector<bool> &err_stack)> post,
           std::vector<bool> &err_stack);

  /**
   * @brief post-order callback function, used to collect & populate sym table
   * with information about global variables and functions. In addition it also
   * creates scopes for all global functions and specifies allowed function
   * scope for all identifiers found in the function block.
   *
   * @param node ast node returned from `#traverse`
   * @param err_stack error stack returned from `#traverse`
   */
  void globals_post_order_pass_cb(ASTNode *node, std::vector<bool> &err_stack);

  /**
   * @brief post-order callback function, used to populate the symtable. In
   * addition it performs lookups to make sure identifiers haven't been
   * re-declared multiple times or if they exist.
   *
   * @param node ast node returned from `#traverse`
   * @param err_stack error stack returned from `#traverse`
   */
  void sym_table_post_pass_cb(ASTNode *node, std::vector<bool> &err_stack);

  /**
   * @brief post-order callback function, used to perform type checking of
   * functions, identifiers, expressions, etc.
   *
   * @param node ast node returned from `#traverse`
   * @param err_stack error stack returned from `#traverse`
   */
  void type_checking_post_order_pass_cb(ASTNode *node,
                                        std::vector<bool> &err_stack);

  /**
   * @brief pre-order callback function, used to determine block nesting level.
   *
   * @param node ast node returned from `#traverse`
   * @param err_stack error stack returned from `#traverse`
   */
  void enter_scope_cb(ASTNode *node, std::vector<bool> &err_stack);

  /**
   * @brief pre-order callback function, used to enter function scopes
   *
   * @param node ast node returned from `#traverse`
   * @param err_stack error stack returned from `#traverse`
   */
  void build_scope_cb(ASTNode *node, std::vector<bool> &err_stack);

  /**
   * @brief perform recursive expression validation based on expected types,
   * predefined in constructor
   *
   * @param expr expression to check
   * @param expected_tyes types expect from the expression
   * @return true if expression is valid
   * @return false otherwise
   */
  bool validate_expr(ASTNode *expr, expr_list_t expected_tyes);

  /**
   * @brief print semantic error message to stderr
   *
   * @param msg message to display
   * @param linenum line number where the message ocurred
   */
  void semantic_error(std::string msg, int linenum);

  /**
   * @brief print semantic error message to stderr. used when linenumber isn't
   * needed/can't be found
   *
   * @param msg message to display
   */
  void semantic_error(std::string msg);
};

#endif /* SEMANTIC_ANALYZER_H */