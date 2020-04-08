#include "CodeGenerator.hpp"

void CodeGenerator::generate_wasm(std::ostream &out) {
  this->traverse(ast.get(), nullptr,
                 std::bind(&CodeGenerator::build_string_table_post_traversal_cb,
                           this, std::placeholders::_1, std::placeholders::_2),
                 out);

  this->traverse(ast.get(),
                 std::bind(&CodeGenerator::codegen_pre_traversal_cb, this,
                           std::placeholders::_1, std::placeholders::_2),
                 std::bind(&CodeGenerator::codegen_post_traversal_cb, this,
                           std::placeholders::_1, std::placeholders::_2),
                 out);
}

void CodeGenerator::traverse(
    ASTNode *node, std::function<void(ASTNode *n, std::ostream &out)> pre,
    std::function<void(ASTNode *n, std::ostream &out)> post,
    std::ostream &out) {
  if (pre != nullptr) {
    pre(node, out);
  }

  for (auto *next : node->children) {
    traverse(next, pre, post, out);
  }

  if (post != nullptr) {
    post(node, out);
  }
}

/**
 * @brief Generate a string table that will be inserted in the generated WASM
 * code
 *
 * @param node AST node
 */
void CodeGenerator::build_string_table_post_traversal_cb(ASTNode *node,
                                                         std::ostream &out) {
  switch (node->type) {
  case Node::string:
    str_table->define(node->value);
    break;
  default:
    break;
  }
}

void CodeGenerator::codegen_pre_traversal_cb(ASTNode *node, std::ostream &out) {
  switch (node->type) {
  case Node::program: {
    out << "(module\n";
    out << "  (import \"host\" \"exit\" (func $exit))\n";
    out << "  (import \"host\" \"getchar\" (func $getchar (result i32)))\n";
    out << "  (import \"host\" \"getchar\" (func $putchar (param i32)))\n";
    out << "  (start $main)\n";
    // std::ifstream runtime("src/lib/runtime.wat");
    // if (runtime.is_open()) {
    //   out << runtime.rdbuf();
    //   out << "\n";
    // }

    out << generate_vars("global");

    break;
  }
  case Node::main_func_decl:
  case Node::function_decl: {
    auto id = node->find_first(Node::id);
    auto type = node->children[0];
    auto formal_params = node->find_first(Node::formal_params);
    auto fun_sym = sym_table->find_function(id->value);

    out << "  (func " << fun_sym->wasm_name;

    // print formal args
    for (auto formal : formal_params->children) {
      auto id = formal->find_first(Node::id);
      out << " (param "
          << "$" << id->value << " i32)";
    }

    // print the return type (result i32)
    if (type->type == Node::int_t || type->type == Node::boolean_t) {
      out << " (result i32)";
    }
    out << "\n";

    // print all the local variables at the very beginning of the function
    out << generate_vars(id->value);

    break;
  }
  default:
    break;
  }
}

void CodeGenerator::codegen_post_traversal_cb(ASTNode *node,
                                              std::ostream &out) {
  switch (node->type) {
  case Node::program: {
    out << "\n";
    out << "  ;;\n";
    out << "  ;; STRINGS\n";
    out << "  ;;\n";
    out << str_table->build_wasm_code("  ") << "\n";
    out << ")\n";
    break;
  }
  case Node::main_func_decl:
  case Node::function_decl:
    out << "  )\n";
    break;
  default:
    break;
  }
}
