#ifndef AST_H
#define AST_H

#include <iostream>
#include <string>
#include <vector>

namespace yy {

struct ast_node_t {
  std::string value;

  // node info variables
  std::string type;
  std::string attr;
  // TODO: line num support
  // int lineno;

  // ast children
  std::vector<ast_node_t *> children;

  bool is_num() { return type == "number"; }
};

} // namespace yy

using ast_node_t = yy::ast_node_t;

inline void print_ast_node(std::ostream &os, const ast_node_t &node,
                           int depth) {
  if (depth >= 10) {
    // TODO: debug only
    return;
  }

  for (int i = 0; i < depth; ++i)
    os << "|\t";

  os << node.value << "{ type: " << node.type << " attr: " << node.attr << " }"
     << "\n";

  for (auto *n : node.children) {
    print_ast_node(os, *n, depth + 1);
  }
}

inline std::ostream &operator<<(std::ostream &os, const ast_node_t &node) {
  print_ast_node(os, node, 0);
  return os;
}

#endif /* AST_H */
