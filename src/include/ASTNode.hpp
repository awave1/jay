#ifndef AST_H
#define AST_H

#include <algorithm>
#include <functional>
#include <iostream>
#include <string>
#include <vector>

namespace yy {

/**
 * @brief enum class that contains all possible node types in the AST
 */
enum class Node {
  program,
  main_func_decl,
  global_var_decl,
  global_func_var_decl,
  function_decl,
  formal_params,
  actual_params,
  block,
  variable_decl,
  func_variable_declaration,

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

/**
 * @brief ASTNode is the definition of Abstract Syntax Tree for J--. Each
 * node in the tree contains some of the following information:
 *   - node type - the type of the node, defined using ASTNode::Note
 *   - string value - value associated with the node, e.g. numberical value of
 * an int
 *   - linenum - the line number where a token can be found
 */
struct ASTNode {
public:
  Node type;
  std::string value;
  int linenum;

  // ast children
  std::vector<ASTNode *> children;

  bool is_while_block = false;
  bool is_return_block = false;
  bool is_formal_param = false;
  bool is_function_id = false;

  bool can_generate_wasm_getter = false;

  std::string function_name;
  Node expected_type;

  void traverse(std::function<void(ASTNode *n)> &pre,
                std::function<void(ASTNode *n)> &post) {
    _traverse(this, pre, post);
  }

  /**
   * @brief find all direct children of specified type
   *
   * @param node_type type of node to be found
   * @return std::vector<ASTNode *> all nodes of specified type
   */
  std::vector<ASTNode *> find_all(Node node_type) {
    std::vector<ASTNode *> res;
    // filter direct children by desired node type
    // copy_if takes in an array of children and inserts matched elements
    // into res vector
    std::copy_if(children.begin(), children.end(), std::back_inserter(res),
                 [&node_type](ASTNode *n) { return n->type == node_type; });

    return res;
  }

  /**
   * @brief finds all nodes with specified type and returns first, if found
   * nodes exist
   *
   * @param node_type type of node to be found
   * @return ASTNode* single node, if it exists, nullptr otherwise
   */
  ASTNode *find_first(Node node_type) {
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
   * @return std::vector<ASTNode *> all possible nodes of specified node type
   */
  std::vector<ASTNode *> find_recursive(Node node_type) {
    auto res = std::vector<ASTNode *>();
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

  bool is_bool_expr() const {
    return type == Node::lt_op || type == Node::lteq_op ||
           type == Node::gt_op || type == Node::gteq_op ||
           type == Node::eqeq_op || type == Node::noteq_op ||
           type == Node::not_op || type == Node::bin_and_op ||
           type == Node::bin_or_op;
  }

  bool is_num_expr() const {
    return type == Node::add_op || type == Node::sub_op ||
           type == Node::mul_op || type == Node::div_op || type == Node::mod_op;
  }

  /**
   * @brief check if a node has value set
   *
   * @return true if it has value st
   * @return false otherwise
   */
  bool has_value() const { return !value.empty(); }

  bool is_const() const {
    return (type == Node::int_t || type == Node::boolean_t) && has_value();
  }

  bool is_assignment_expr() { return this->next_child()->type == Node::eq_op; }

  ASTNode *next_child() { return this->children.front(); }

  /**
   * @brief Get the type of the object
   *
   * @return std::string human readable representation of the type
   */
  std::string type_to_str() const { return type_to_str(type); }

  /**
   * @brief pretty print given ASTNode (and its children)
   *
   * @param os output stream
   * @param node node to print
   * @param depth value that specifies how deep down the tree should it go
   */
  void print_ast_node(std::ostream &os, const ASTNode &node, int depth) const {
    for (int i = 0; i < depth; i++) {
      os << "."
         << "  ";
    }

    os << node.type_to_str();
    os << " { ";

    if (node.has_value()) {
      os << "value: '" << node.value << "'";
    }

    if (node.type == Node::block) {
      os << std::boolalpha << "is_while_block: " << node.is_while_block
         << " is_return_block: " << node.is_return_block;
    }

    if (node.is_num_expr()) {
      os << " expected_type: " << type_to_str(expected_type);
    }

    if (node.type == Node::id) {
      os << " function_name: " << node.function_name;
    }

    if (node.linenum != 0) {
      os << " line: " << node.linenum;
    }

    os << " }" << std::endl;

    for (auto *n : node.children) {
      print_ast_node(os, *n, depth + 1);
    }
  }

  friend std::ostream &operator<<(std::ostream &os, const yy::ASTNode &node) {
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
  void _find_recursive(ASTNode *node, Node node_type,
                       std::vector<ASTNode *> &res) {
    for (auto *next : node->children) {
      _find_recursive(next, node_type, res);
    }

    std::copy_if(node->children.begin(), node->children.end(),
                 std::back_inserter(res),
                 [&node_type](ASTNode *n) { return n->type == node_type; });
  }

  void _traverse(ASTNode *node, std::function<void(ASTNode *n)> &pre,
                 std::function<void(ASTNode *n)> &post) {
    if (pre != nullptr) {
      pre(node);
    }

    for (auto *next : node->children) {
      _traverse(next, pre, post);
    }

    if (post != nullptr) {
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
    case Node::func_variable_declaration:
      return "func_variable_declaration";
    case Node::global_func_var_decl:
      return "global_func_variable_declaration";
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
      return "statement_expr";
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

inline std::string get_str_for_type(yy::Node type) {
  switch (type) {
  case yy::Node::program:
    return "program";
  case yy::Node::main_func_decl:
    return "main_func_decl";
  case yy::Node::global_var_decl:
    return "global_var_decl";
  case yy::Node::func_variable_declaration:
    return "func_variable_declaration";
  case yy::Node::function_decl:
    return "function_decl";
  case yy::Node::formal_params:
    return "formal_params";
  case yy::Node::actual_params:
    return "actual_params";
  case yy::Node::block:
    return "block";
  case yy::Node::variable_decl:
    return "variable_decl";
  case yy::Node::null_statement:
    return "null_statement";
  case yy::Node::break_statement:
    return "break";
  case yy::Node::return_statement:
    return "return";
  case yy::Node::if_statement:
    return "if";
  case yy::Node::else_statement:
    return "else";
  case yy::Node::if_else_statement:
    return "if... else";
  case yy::Node::while_statement:
    return "while";
  case yy::Node::id:
    return "id";
  case yy::Node::statement_expr:
    return "statement_expt";
  case yy::Node::function_call:
    return "function_call";
  case yy::Node::formal:
    return "formal";
  case yy::Node::add_op:
    return "+";
  case yy::Node::sub_op:
    return "-";
  case yy::Node::mul_op:
    return "*";
  case yy::Node::div_op:
    return "/";
  case yy::Node::mod_op:
    return "%";
  case yy::Node::eq_op:
    return "=";
  case yy::Node::not_op:
    return "!";
  case yy::Node::eqeq_op:
    return "==";
  case yy::Node::noteq_op:
    return "!=";
  case yy::Node::lt_op:
    return "<";
  case yy::Node::gt_op:
    return ">";
  case yy::Node::lteq_op:
    return "<=";
  case yy::Node::gteq_op:
    return ">=";
  case yy::Node::bin_and_op:
    return "&&";
  case yy::Node::bin_or_op:
    return "||";
  case yy::Node::int_t:
    return "int";
  case yy::Node::number:
    return "number";
  case yy::Node::string:
    return "string";
  case yy::Node::boolean_t:
    return "boolean";
  case yy::Node::void_t:
    return "void";
  default:
    return "";
  }
}

#endif /* AST_H */
