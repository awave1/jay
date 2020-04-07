#ifndef CODE_GENERATOR_HPP
#define CODE_GENERATOR_HPP

#include "ASTNode.hpp"
#include "StringTable.hpp"
#include "SymTable.hpp"
#include <memory>

using namespace yy;

class CodeGenerator {
public:
  CodeGenerator(std::shared_ptr<ASTNode> ast,
                std::shared_ptr<SymTable> sym_table)
      : ast(ast), sym_table(sym_table) {
    str_table = std::unique_ptr<StringTable>(new StringTable());
  };

private:
  std::shared_ptr<ASTNode> ast;
  std::shared_ptr<SymTable> sym_table;
  std::unique_ptr<StringTable> str_table;
};

#endif /* CODE_GENERATOR_HPP */