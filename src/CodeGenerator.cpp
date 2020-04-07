#include "CodeGenerator.hpp"

std::string CodeGenerator::generate_wasm() {

  this->traverse(ast.get(), nullptr,
                 std::bind(&CodeGenerator::build_string_table_post_traversal_cb,
                           this, std::placeholders::_1));

  return "<CODE>";
}

void CodeGenerator::traverse(ASTNode *node, std::function<void(ASTNode *n)> pre,
                             std::function<void(ASTNode *n)> post) {
  if (pre != nullptr) {
    pre(node);
  }

  for (auto *next : node->children) {
    traverse(next, pre, post);
  }

  if (post != nullptr) {
    post(node);
  }
}

void CodeGenerator::build_string_table_post_traversal_cb(ASTNode *n) {
  switch (n->type) {
  case Node::string:
    str_table->define(n->value);
    break;
  default:
    break;
  }
}
