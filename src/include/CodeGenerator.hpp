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
                std::shared_ptr<SymTable> sym_table)
      : ast(ast), sym_table(sym_table) {
    str_table = std::unique_ptr<StringTable>(new StringTable());
  };

  /**
   * @brief
   *
   * @return std::string
   */
  void generate_wasm(std::ostream &out);

private:
  std::shared_ptr<ASTNode> ast;
  std::shared_ptr<SymTable> sym_table;
  std::unique_ptr<StringTable> str_table;

  void traverse(ASTNode *node,
                std::function<void(ASTNode *n, std::ostream &out)> pre,
                std::function<void(ASTNode *n, std::ostream &out)> post,
                std::ostream &out);

  std::string generate_vars(std::string scope_name) {
    std::string out = "";
    bool is_global = scope_name == "global";

    auto scope = sym_table->get_scope(scope_name);
    for (auto const &[name, sym] : scope) {
      if (sym->kind == "variable") {
        if (is_global) {
          out += "  (global ";
          out += sym->wasm_name;
          out += " (mut i32) (i32.const 0)";
          out += ")\n";
        } else {
          out += "    (local ";
          out += sym->wasm_name;
          out += " (i32.const 0)";
          out += ")\n";
        }
      }
    }

    return out;
  }

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