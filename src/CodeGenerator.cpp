#include "CodeGenerator.hpp"

void CodeGenerator::generate_wasm() {
  this->traverse(ast.get(), nullptr,
                 std::bind(&CodeGenerator::build_string_table_post_traversal_cb,
                           this, std::placeholders::_1, std::placeholders::_2),
                 this->out);

  this->traverse(ast.get(),
                 std::bind(&CodeGenerator::codegen_pre_traversal_cb, this,
                           std::placeholders::_1, std::placeholders::_2),
                 std::bind(&CodeGenerator::codegen_post_traversal_cb, this,
                           std::placeholders::_1, std::placeholders::_2),
                 this->out);
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
    out << "  (import \"host\" \"putchar\" (func $putchar (param i32)))\n";
    out << "  (start $main)\n";
    out << "  (memory 1)\n";

    // inject runtime functions
    // inject_runtime();

    generate_vars("global");

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
    generate_vars(id->value);

    break;
  }
  case Node::statement_expr: {
    auto statement = node->children[0];

    if (statement->type == Node::eq_op) {
      auto id = statement->children[0];
      auto right_op = statement->children[1];

      if (right_op->type == Node::function_call) {
        auto fun_sym =
            sym_table->find_function(right_op->find_first(Node::id)->value);
        auto id_sym = sym_table->lookup(id->value, fun_sym->name);
        auto var_scope = id_sym->scope_level > 1 ? "local" : "global";

        /**
         * call $foo
         * local.set $i
         */

        // TODO: but that doesnt account for nested func calls

        // build a stack of nested function calls
        //

        out << "    "
            << "call " << fun_sym->wasm_name << "\n";
        out << "    " << var_scope << ".set " << id_sym->wasm_name << "\n";
      }
    }

    break;
  }
  case Node::function_call: {
    // auto id = node->find_first(Node::id);
    // auto actual_params = node->find_first(Node::actual_params);
    // auto fun_sym = sym_table->find_function(id->value);

    // for (auto actual : actual_params->children) {
    //   if (actual->type == Node::string) {
    //     // strings are a special case because we need to pass two params to
    //     it
    //     // in wasm - an offset and a length of the string
    //     auto entry = str_table->lookup(actual->value);
    //     out << "    "
    //         << "i32.const " << entry.offset << "\n"
    //         << "    "
    //         << "i32.const " << entry.length << "\n";
    //   } else if (actual->type == Node::boolean_t) {
    //     // pass 1 if true or 0 if false
    //     out << "    "
    //         << "i32.const " << (actual->value == "true" ? "1" : "0") << "\n";
    //   } else if (actual->type == Node::int_t) {
    //     out << "    "
    //         << "i32.const " << actual->value << "\n";
    //   }
    // }

    // out << "    "
    //     << "call " << fun_sym->wasm_name << "\n";
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

void CodeGenerator::generate_vars(std::string scope_name) {
  bool is_global = scope_name == "global";

  auto scope = sym_table->get_scope(scope_name);
  for (auto const &[name, sym] : scope) {
    if (sym->kind == "variable") {
      if (is_global) {
        out << "  (global ";
        out << sym->wasm_name;
        out << " (mut i32) (i32.const 0)";
        out << ")\n";
      } else {
        out << "    (local ";
        out << sym->wasm_name;
        out << " (i32.const 0)";
        out << ")\n";
      }
    }
  }
}

void CodeGenerator::build_function_call() {
  // auto id = node->find_first(Node::id);
  // auto actual_params = node->find_first(Node::actual_params);
  // auto fun_sym = sym_table->find_function(id->value);

  // for (auto actual : actual_params->children) {
  //   if (actual->type == Node::string) {
  //     // strings are a special case because we need to pass two params to
  //     it
  //         // in wasm - an offset and a length of the string
  //         auto entry = str_table->lookup(actual->value);
  //     out << "    "
  //         << "i32.const " << entry.offset << "\n"
  //         << "    "
  //         << "i32.const " << entry.length << "\n";
  //   } else if (actual->type == Node::boolean_t) {
  //     // pass 1 if true or 0 if false
  //     out << "    "
  //         << "i32.const " << (actual->value == "true" ? "1" : "0") << "\n";
  //   } else if (actual->type == Node::int_t) {
  //     out << "    "
  //         << "i32.const " << actual->value << "\n";
  //   }
  // }

  // out << "    "
  //     << "call " << fun_sym->wasm_name << "\n";
}

void CodeGenerator::inject_runtime() {
  std::ifstream runtime("src/lib/runtime.wat");
  if (runtime.is_open()) {
    out << runtime.rdbuf();
    out << "\n";
  }
}