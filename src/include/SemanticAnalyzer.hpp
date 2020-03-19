#ifndef SEMANTIC_ANALYZER_H
#define SEMANTIC_ANALYZER_H

#include "ast.hpp"
#include <memory>

using namespace yy;

class SemanticAnalyzer {
public:
  SemanticAnalyzer(std::shared_ptr<ast_node_t> ast) : ast(ast) {}
  ~SemanticAnalyzer() {}

  std::shared_ptr<ast_node_t> get_ast();

  bool validate();

private:
  std::shared_ptr<ast_node_t> ast;
};

#endif /* SEMANTIC_ANALYZER_H */