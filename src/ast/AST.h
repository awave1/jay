#pragma once
#include <iostream>
#include <string>
#include <vector>

namespace yy {
struct ASTNode {
  std::string name;
  std::vector<ASTNode *> children;
};
} // namespace yy

using ASTNode = yy::ASTNode;
inline void printAST(ASTNode *node, int depth = 0) {
  for (int i = 0; i < depth; ++i) {
    std::cout << "\t";
  }
  std::cout << node->name << "\n";
  for (auto *child : node->children) {
    printAST(child, depth + 1);
  }
}
