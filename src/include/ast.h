#ifndef AST_H
#define AST_H

#include <iostream>
#include <string>
#include <vector>

struct ASTNode {
  std::string val;
  std::vector<ASTNode *> children;
};

inline void printAstNode(std::ostream &os, ASTNode const &node, int tabDepth) {
  for (int i = 0; i < tabDepth; ++i)
    os << "|\t";

  os << "| " << node.val << "\n";

  for (auto *n : node.children) {
    printAstNode(os, *n, tabDepth + 1);
  }
}
inline std::ostream &operator<<(std::ostream &os, ASTNode const &node) {
  printAstNode(os, node, 0);
  return os;
}

#endif /* AST_H */