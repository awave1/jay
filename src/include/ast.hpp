#ifndef AST_H
#define AST_H

#include <algorithm>
#include <functional>
#include <iostream>
#include <string>
#include <vector>

namespace yy {

/**
 * @brief ast_node_t is the definition of Abstract Syntax Tree for J--. Each
 * node in the tree contains some of the following information:
 *   - node type - the type of the node, defined using ast_node_t::Note
 *   - string value - value associated with the node, e.g. numberical value of
 * an int
 *   - linenum - the line number where a token can be found
 */
struct ast_node_t {
  /**
   * @brief enum class that contains all possible node types in the AST
   */
  enum class Node {
    program,
    main_func_decl,
    global_var_decl,
    function_decl,
    formal_params,
    actual_params,
    block,
    variable_decl,

    null_statement,
    break_statement,
    return_statement,
    if_statement,
    else_statement,
    if_else_statement,
    while_statement,

    statement_expr,
    function_call,

    id,
    formal,

    add_op,
    sub_op,
    mul_op,
    div_op,
    mod_op,
    eq_op,
    not_op,
    eqeq_op,
    noteq_op,
    lt_op,
    gt_op,
    lteq_op,
    gteq_op,
    bin_and_op,
    bin_or_op,

    int_t,
    boolean_t,
    void_t,
    number,
    string,
  };

  Node type;
  std::string value;
  int linenum;

  // ast children
  std::vector<ast_node_t *> children;

  bool is_while_block = false;
  bool is_return_block = false;

  /**
   * Need to do several passes of the AST
   *
   * Pass 1: Post Order Pass
   *   - Collect information about global declarations in the program
   *      - build global scope in the symbol table?
   *
   * Notes:
   *     Propagate type information and store information about global
   *     identifiers. Need symbol table -> using a scope stack. At every point
   *     during the compilation, there will be a symbol table inserted at the
   *     *top* of the stack, and always insert identifiest at the
   *     *topmost scope*, to find identifiers search from the top scope and
   *     go down if no id found, it's an error.
   *     Outermost layer (0) contains predefined stuff, layer (1) contains
   *     globals layers (2-...some max scope level) contain local scope. Every
   *     layer points to a hashmap. In the hashmap have entries about predefined
   *     identifiers. Note that scope 0 should be predefined before the
   *     compilation. Scope 1, the global scope, all the globals are added there
   *     in the hash table.
   *
   *     Whenever you arrive at the type (Node type), propagate up the
   *     information so it's available at the higher node in the tree
   *
   *
   * Pass 2: Pre-Post Pass
   *   - filling in the symbol table information fully!
   *   - determine what identifier nodes point to
   *      - what symbols they refer to, even nested down in the tree
   *
   * Notes:
   *   Open and close the scopes as tree is traversed. Ensure all the
   *   declaration are within the scope. Here you need to look for declarations
   *   (params, variables).
   *
   * Pass 3: Post order pass
   *   - Full type checking
   *   - using synthesized attrs coming from leaves of the tree, and propagete
   * type information
   *
   * Notes:
   *   Catch arithmetic and boolean types via a table
   *   (e.g. table["=="] == [('int', 'int', 'boolean')])
   *
   * Pass 4: Pre-Post traversal
   *   - catch-all traversal, trying to catch things missed before
   */
  bool traverse(std::function<void(ast_node_t *n)> &pre,
                std::function<void(ast_node_t *n)> &post) {
    _traverse(this, pre, post);
    return true;
  }

  /**
   * @brief find all direct children of specified type
   *
   * @param node_type type of node to be found
   * @return std::vector<ast_node_t *> all nodes of specified type
   */
  std::vector<ast_node_t *> find_all(Node node_type) {
    std::vector<ast_node_t *> res;
    // filter direct children by desired node type
    // copy_if takes in an array of children and inserts matched elements
    // into res vector
    std::copy_if(children.begin(), children.end(), std::back_inserter(res),
                 [&node_type](ast_node_t *n) { return n->type == node_type; });

    return res;
  }

  /**
   * @brief finds all nodes with specified type and returns first, if found
   * nodes exist
   *
   * @param node_type type of node to be found
   * @return ast_node_t* single node, if it exists, nullptr otherwise
   */
  ast_node_t *find_first(Node node_type) {
    auto nodes = find_all(node_type);
    if (!nodes.empty()) {
      return nodes[0];
    }

    return nullptr;
  }

  /**
   * @brief search for nodes of specified type recursively and return all
   * matches
   *
   * @param node_type type of node to be found
   * @return std::vector<ast_node_t *> all possible nodes of specified node type
   */
  std::vector<ast_node_t *> find_recursive(Node node_type) {
    auto res = std::vector<ast_node_t *>();
    _find_recursive(this, node_type, res);
    return res;
  }

  /**
   * @brief check if a node is numeric
   *
   * @return true if it is a number
   * @return false otherwise
   */
  bool is_num() const { return type == Node::number; }

  /**
   * @brief check if a node has value set
   *
   * @return true if it has value st
   * @return false otherwise
   */
  bool has_value() const { return !value.empty(); }

  /**
   * @brief Get the type of the object
   *
   * @return std::string human readable representation of the type
   */
  std::string get_type() const { return type_to_str(type); }

  /**
   * @brief pretty print given ast_node_t (and its children)
   *
   * @param os output stream
   * @param node node to print
   * @param depth value that specifies how deep down the tree should it go
   */
  void print_ast_node(std::ostream &os, const ast_node_t &node,
                      int depth) const {
    for (int i = 0; i < depth; i++) {
      os << "."
         << "  ";
    }

    os << node.get_type();
    os << " { ";

    if (node.has_value()) {
      os << "value: '" << node.value << "'";
    }

    if (node.type == Node::block) {
      os << std::boolalpha << "is_while_block: " << node.is_while_block
         << " is_return_block: " << node.is_return_block;
    }

    if (node.linenum != 0) {
      os << " line: " << node.linenum;
    }

    os << " }" << std::endl;

    for (auto *n : node.children) {
      print_ast_node(os, *n, depth + 1);
    }
  }

  friend std::ostream &operator<<(std::ostream &os,
                                  const yy::ast_node_t &node) {
    node.print_ast_node(os, node, 0);
    return os;
  }

private:
  /**
   * @brief helper method for recursive node finder. starts searching from given
   * node and writes results to specified vector
   *
   * @param node node to start search from
   * @param node_type desired node type
   * @param res resulting vector with all found nodes
   */
  void _find_recursive(ast_node_t *node, ast_node_t::Node node_type,
                       std::vector<ast_node_t *> &res) {
    if (node->children.empty()) {
      return;
    }

    std::size_t i = 0;
    do {
      std::copy_if(
          node->children.begin(), node->children.end(), std::back_inserter(res),
          [&node_type](ast_node_t *n) { return n->type == node_type; });
      auto *next = node->children[i];
      _find_recursive(next, node_type, res);

      i++;
    } while (i < node->children.size());
  }

  void _traverse(ast_node_t *node, std::function<void(ast_node_t *n)> &pre,
                 std::function<void(ast_node_t *n)> &post) {
    if (node->children.empty()) {
      return;
    }

    if (pre != nullptr) {
      // TODO: Pre-order traversal
      pre(node);
    }

    std::size_t i = 0;
    do {
      auto *next = node->children[i];

      _traverse(next, pre, post);

      i++;
    } while (i < node->children.size());

    if (post != nullptr) {
      // TODO: Post-order traversal
      post(node);
    }
  }

  std::string type_to_str(Node type) const {
    switch (type) {
    case Node::program:
      return "program";
    case Node::main_func_decl:
      return "main_func_decl";
    case Node::global_var_decl:
      return "global_var_decl";
    case Node::function_decl:
      return "function_decl";
    case Node::formal_params:
      return "formal_params";
    case Node::actual_params:
      return "actual_params";
    case Node::block:
      return "block";
    case Node::variable_decl:
      return "variable_decl";
    case Node::null_statement:
      return "null_statement";
    case Node::break_statement:
      return "break";
    case Node::return_statement:
      return "return";
    case Node::if_statement:
      return "if";
    case Node::else_statement:
      return "else";
    case Node::if_else_statement:
      return "if_else";
    case Node::while_statement:
      return "while";
    case Node::id:
      return "id";
    case Node::statement_expr:
      return "statement_expt";
    case Node::function_call:
      return "function_call";
    case Node::formal:
      return "formal";
    case Node::add_op:
      return "+";
    case Node::sub_op:
      return "-";
    case Node::mul_op:
      return "*";
    case Node::div_op:
      return "/";
    case Node::mod_op:
      return "%";
    case Node::eq_op:
      return "=";
    case Node::not_op:
      return "!";
    case Node::eqeq_op:
      return "==";
    case Node::noteq_op:
      return "!=";
    case Node::lt_op:
      return "<";
    case Node::gt_op:
      return ">";
    case Node::lteq_op:
      return "<=";
    case Node::gteq_op:
      return ">=";
    case Node::bin_and_op:
      return "&&";
    case Node::bin_or_op:
      return "||";
    case Node::int_t:
      return "int";
    case Node::number:
      return "number";
    case Node::string:
      return "string";
    case Node::boolean_t:
      return "boolean";
    case Node::void_t:
      return "void";
    default:
      return "";
    }
  }
};

} // namespace yy

inline std::string get_str_for_type(yy::ast_node_t::Node type) {
  switch (type) {
  case yy::ast_node_t::Node::program:
    return "program";
  case yy::ast_node_t::Node::main_func_decl:
    return "main_func_decl";
  case yy::ast_node_t::Node::global_var_decl:
    return "global_var_decl";
  case yy::ast_node_t::Node::function_decl:
    return "function_decl";
  case yy::ast_node_t::Node::formal_params:
    return "formal_params";
  case yy::ast_node_t::Node::actual_params:
    return "actual_params";
  case yy::ast_node_t::Node::block:
    return "block";
  case yy::ast_node_t::Node::variable_decl:
    return "variable_decl";
  case yy::ast_node_t::Node::null_statement:
    return "null_statement";
  case yy::ast_node_t::Node::break_statement:
    return "break";
  case yy::ast_node_t::Node::return_statement:
    return "return";
  case yy::ast_node_t::Node::if_statement:
    return "if";
  case yy::ast_node_t::Node::else_statement:
    return "else";
  case yy::ast_node_t::Node::if_else_statement:
    return "if_else";
  case yy::ast_node_t::Node::while_statement:
    return "while";
  case yy::ast_node_t::Node::id:
    return "id";
  case yy::ast_node_t::Node::statement_expr:
    return "statement_expt";
  case yy::ast_node_t::Node::function_call:
    return "function_call";
  case yy::ast_node_t::Node::formal:
    return "formal";
  case yy::ast_node_t::Node::add_op:
    return "+";
  case yy::ast_node_t::Node::sub_op:
    return "-";
  case yy::ast_node_t::Node::mul_op:
    return "*";
  case yy::ast_node_t::Node::div_op:
    return "/";
  case yy::ast_node_t::Node::mod_op:
    return "%";
  case yy::ast_node_t::Node::eq_op:
    return "=";
  case yy::ast_node_t::Node::not_op:
    return "!";
  case yy::ast_node_t::Node::eqeq_op:
    return "==";
  case yy::ast_node_t::Node::noteq_op:
    return "!=";
  case yy::ast_node_t::Node::lt_op:
    return "<";
  case yy::ast_node_t::Node::gt_op:
    return ">";
  case yy::ast_node_t::Node::lteq_op:
    return "<=";
  case yy::ast_node_t::Node::gteq_op:
    return ">=";
  case yy::ast_node_t::Node::bin_and_op:
    return "&&";
  case yy::ast_node_t::Node::bin_or_op:
    return "||";
  case yy::ast_node_t::Node::int_t:
    return "int";
  case yy::ast_node_t::Node::number:
    return "number";
  case yy::ast_node_t::Node::string:
    return "string";
  case yy::ast_node_t::Node::boolean_t:
    return "boolean";
  case yy::ast_node_t::Node::void_t:
    return "void";
  default:
    return "";
  }
}

#endif /* AST_H */
