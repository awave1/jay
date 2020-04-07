#ifndef CODE_GENERATOR_HPP
#define CODE_GENERATOR_HPP

#include "ASTNode.hpp"
#include "StringTable.hpp"
#include "SymTable.hpp"
#include <functional>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

using namespace yy;

class CodeGenerator {
public:
  CodeGenerator(std::shared_ptr<ASTNode> ast,
                std::shared_ptr<SymTable> sym_table)
      : ast(ast), sym_table(sym_table) {
    str_table = std::unique_ptr<StringTable>(new StringTable());
  };

  /**
   * @brief
   *
   * @return std::string
   */
  std::string generate_wasm();

private:
  std::shared_ptr<ASTNode> ast;
  std::shared_ptr<SymTable> sym_table;
  std::unique_ptr<StringTable> str_table;

  void traverse(ASTNode *node, std::function<void(ASTNode *n)> pre,
                std::function<void(ASTNode *n)> post);

  void codegen_pre_traversal_cb(ASTNode *n);
  void codegen_post_traversal_cb(ASTNode *n);
  void build_string_table_post_traversal_cb(ASTNode *n);
};

#endif /* CODE_GENERATOR_HPP */