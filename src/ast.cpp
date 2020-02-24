#include "include/ast.h"

ast_node_t *make_new_node(std::string value) {
  ast_node_t *node = new ast_node_t;
  node->value = value;
  return node;
}

ast_node_t *make_new_node(char *name) {
  return make_new_node(std::string(name));
}

void add_child(ast_node_t *node, ast_node_t *child) {
  node->children.push_back(child);
}

std::string ast_to_string(ast_node_t *tree) {
  std::string tree_string = "{";
  tree_string.append(tree->value);
  tree_string.append(" ");
  for (auto i = 0; i < tree->children.size(); i++) {
    tree_string.append(ast_to_string(tree->children[i]));
  }
  tree_string.append("}");
  return tree_string;
}