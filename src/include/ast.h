#ifndef AST_H
#define AST_H

#include <string>
#include <vector>

typedef struct ast_node_t {
  std::string value;
  std::vector<ast_node_t *> children;
} ast_node_t;

ast_node_t *make_new_node(std::string name);
ast_node_t *make_new_node(char *name);
void add_child(ast_node_t *node, ast_node_t *child);
std::string ast_to_string(ast_node_t *tree);

inline std::ostream &operator<<(std::ostream &os, ast_node_t *node) {
  return os << ast_to_string(node);
}

#endif /* AST_H */