#ifndef AST_H
#define AST_H

#include <string>
#include <vector>

struct ASTNode {
  std::string value;
  std::vector<ASTNode *> children;
};

inline std::ostream &operator<<(std::ostream &os, ASTNode const &node) {
  return os;
}

#endif /* AST_H */