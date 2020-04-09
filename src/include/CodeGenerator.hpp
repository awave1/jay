#ifndef CODE_GENERATOR_HPP
#define CODE_GENERATOR_HPP

#include "ASTNode.hpp"
#include "StringTable.hpp"
#include "SymTable.hpp"
#include <fstream>
#include <functional>
#include <iostream>
#include <memory>
#include <ostream>
#include <stdexcept>
#include <string>
#include <vector>

using namespace yy;

class CodeGenerator {
public:
  CodeGenerator(std::shared_ptr<ASTNode> ast,
                std::shared_ptr<SymTable> sym_table, std::ostream &out)
      : ast(ast), sym_table(sym_table), out(out) {
    str_table = std::unique_ptr<StringTable>(new StringTable());
  };

  /**
   * @brief
   *
   * @return std::string
   */
  void generate_wasm();

private:
  std::shared_ptr<ASTNode> ast;
  std::shared_ptr<SymTable> sym_table;
  std::unique_ptr<StringTable> str_table;
  std::ostream &out;

  void traverse(ASTNode *node,
                std::function<void(ASTNode *n, std::ostream &out)> pre,
                std::function<void(ASTNode *n, std::ostream &out)> post,
                std::ostream &out);

  void generate_vars(std::string scope_name);

  void build_function_call();

  void inject_runtime();

  void codegen_pre_traversal_cb(ASTNode *node, std::ostream &out);
  void codegen_post_traversal_cb(ASTNode *node, std::ostream &out);

  /**
   * @brief Generate a string table that will be inserted in the generated WASM
   * code
   *
   * @param node AST node
   */
  void build_string_table_post_traversal_cb(ASTNode *node, std::ostream &out);
};

#endif /* CODE_GENERATOR_HPP */