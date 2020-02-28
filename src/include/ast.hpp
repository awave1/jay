#ifndef AST_H
#define AST_H

#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

namespace yy {

struct ast_node_t {
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

  std::vector<ast_node_t *> find_all(Node node_type) {
    std::vector<ast_node_t *> res;
    std::copy_if(children.begin(), children.end(), std::back_inserter(res),
                 [&node_type](ast_node_t *n) { return n->type == node_type; });

    return res;
  }

  ast_node_t *find_first(Node node_type) {
    auto nodes = find_all(node_type);
    if (!nodes.empty()) {
      return nodes[0];
    }

    return nullptr;
  }

  std::vector<ast_node_t *> find_recursive(Node node_type) {
    auto res = std::vector<ast_node_t *>();
    _find_recursive(this, node_type, res);
    return res;
  }

  bool is_num() const { return type == Node::number; }

  bool has_value() const { return !value.empty(); }

  std::string get_type() const {
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
    }
  }

private:
  inline void _find_recursive(ast_node_t *node, ast_node_t::Node node_type,
                              std::vector<ast_node_t *> &res) {
    if (node->children.empty()) {
      return;
    }

    for (auto *next : node->children) {
      std::copy_if(
          next->children.begin(), next->children.end(), std::back_inserter(res),
          [&node_type](ast_node_t *n) { return n->type == node_type; });
      _find_recursive(next, node_type, res);
    }
  }
};

} // namespace yy

using ast_node_t = yy::ast_node_t;

inline void find_recursive(ast_node_t *node, ast_node_t::Node node_type,
                           std::vector<ast_node_t *> &res) {
  if (node->children.empty()) {
    return;
  }

  for (auto *next : node->children) {
    std::copy_if(next->children.begin(), next->children.end(),
                 std::back_inserter(res),
                 [&node_type](ast_node_t *n) { return n->type == node_type; });
    find_recursive(next, node_type, res);
  }
}

inline void print_ast_node(std::ostream &os, const ast_node_t &node,
                           int depth) {
  for (int i = 0; i < depth; i++) {
    os << "."
       << "  ";
  }

  os << node.get_type();
  os << " { ";

  if (node.has_value()) {
    os << "value: '" << node.value << "'";
  }

  if (node.linenum != 0) {
    os << " line: " << node.linenum;
  }

  os << " }" << std::endl;

  for (auto *n : node.children) {
    print_ast_node(os, *n, depth + 1);
  }
}

inline std::ostream &operator<<(std::ostream &os, const ast_node_t &node) {
  print_ast_node(os, node, 0);
  return os;
}

#endif /* AST_H */
