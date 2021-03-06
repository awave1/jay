/**
 * @file CodeGenerator.cpp
 * @author Artem Golovin (30018900)
 * @brief Generate WebAssembly text format for J-- code
 */

#include "CodeGenerator.hpp"

/**
 * @brief Generate wasm code and output it to out stream (by default it goes
 * to stdout)
 */
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

/**
 * @brief YATM (Yet Another Traversal Method). Method to traverse AST
 * TODO: make this *actually* generic
 *
 * @param node AST node
 * @param pre pre traversal hook/callback
 * @param post  post traversal hook/callback
 * @param out output stream
 */
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

  auto iter = printer->decorations.find(node);
  if (iter != printer->decorations.end()) {
    if (iter->second.pre != nullptr) {
      iter->second.pre(node);
    }
  }

  switch (node->type) {
  case Node::program: {
    out << printer->add("(module", false) << printer->indent();
    out << printer->line(
        R"((import "host" "exit" (func $exit)))");
    out << printer->line(
        R"((import "host" "putchar" (func $putchar (param i32))))");
    out << printer->line(
        R"((import "host" "getchar" (func $getchar (result i32))))");
    out << printer->line("(memory 1)");

    // TODO: inject runtime functions
    inject_runtime();

    generate_vars("global");
    break;
  }
  case Node::main_func_decl:
  case Node::function_decl: {
    auto *id = node->find_first(Node::id);
    auto *type = node->children[0];
    auto *formal_params = node->find_first(Node::formal_params);
    auto *fun_sym = sym_table->find_function(id->value);

    if (node->type == Node::main_func_decl) {
      // @NOTE: explicitly state start function name because it DOESN'T have to
      // be main
      this->start_func_name = fun_sym->wasm_name;
    }

    out << printer->line("") << printer->line("(func " + fun_sym->wasm_name);

    // print formal args
    for (auto formal : formal_params->children) {
      auto id = formal->find_first(Node::id);
      out << printer->add_param(id->value);
    }

    // print the return type (result i32)
    if (type->type == Node::int_t || type->type == Node::boolean_t) {
      out << printer->add("(result i32)");
    }

    out << printer->line("");

    // print all the local variables at the very beginning of the function
    generate_vars(id->value);

    out << printer->indent();
    out << printer->line("");
    break;
  }
  case Node::if_statement:
  case Node::if_else_statement: {
    out << printer->line("(if") << printer->indent();

    printer->decorations[node->next_child()] = {
        [this](ASTNode *) {
          this->out << this->printer->line("(block (result i32)")
                    << this->printer->indent();
        },
        [this](ASTNode *) {
          this->out << this->printer->dedent() << this->printer->line(")");
        }};

    printer->decorations[node->children[1]] = {
        [this](ASTNode *) {
          this->out << this->printer->line("(then ") << this->printer->indent();
        },
        [this](ASTNode *) {
          this->out << this->printer->dedent() << this->printer->line(")");
        }};

    if (node->type == Node::if_else_statement) {
      printer->decorations[node->children[2]] = {
          [this](ASTNode *) {
            this->out << this->printer->line("(else ")
                      << this->printer->indent();
          },
          [this](ASTNode *) {
            this->out << this->printer->dedent() << this->printer->line(")");
          }};
    }

    break;
  }
  case Node::while_statement: {
    out << printer->line("(block $_block" + get_block_state())
        << printer->indent();
    out << printer->line("(loop $_loop" + get_block_state())
        << printer->indent();

    printer->decorations[node->next_child()] = {
        nullptr, [this](ASTNode *) {
          // this->out << this->printer->dedent() << this->printer->line(")");
          this->out << this->printer->line("i32.eqz");
          this->out << this->printer->line("br_if $_block" +
                                           this->get_block_state());

          next_block_state();
        }};

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
    out << printer->line("");
    out << printer->line(";;");
    out << printer->line(";; STRINGS");
    out << printer->line(";;");
    out << "\n" << str_table->build_wasm_code();

    out << printer->line("(start " + this->start_func_name + ")")
        << printer->dedent() << printer->line(")\n");

    break;
  }
  case Node::main_func_decl: {
    auto *block = node->find_first(Node::block);
    if (!block->children.empty()) {
      auto *last_expr = block->children.back();

      if (last_expr->type == Node::statement_expr &&
          !last_expr->children.empty()) {
        auto *fun_call = last_expr->find_first(Node::function_call);
        auto *fun_id = fun_call->next_child();
        auto *fun_sym = sym_table->find_function(fun_id->value);

        if (fun_sym->type != Node::void_t) {
          out << printer->line("drop");
          out << printer->line("call $halt");
        }
      }

    } else {
      out << printer->line("call $halt");
    }

    out << printer->dedent() << printer->line(")");
    break;
  }
  case Node::function_decl: {
    auto *id = node->find_first(Node::id);
    auto *fun_sym = sym_table->find_function(id->value);

    if (fun_sym->type != Node::void_t) {
      out << printer->line("unreachable");
    }

    out << printer->dedent() << printer->line(")");
    break;
  }
  case Node::id: {
    if (!node->function_name.empty() && !node->is_formal_param &&
        node->can_generate_wasm_getter) {
      auto name = node->value;
      auto sym = sym_table->lookup(name, node->function_name);
      if (sym->is_global()) {
        out << printer->line("") << "global.get"
            << printer->add_name(sym->name);
      } else {
        out << printer->line("") << "local.get" << printer->add_name(sym->name);
      }
    }

    break;
  }
  case Node::function_call: {
    auto id = node->find_first(Node::id);
    auto fun_sym = sym_table->find_function(id->value);
    auto actual_params = node->find_first(Node::actual_params);

    if (fun_sym->name == "prints") {
      for (auto actual : actual_params->children) {
        if (actual->type == Node::string) {
          // strings are a special case because we need to pass two params to it
          // in wasm - an offset and a length of the string
          auto entry = str_table->lookup(actual->value);
          out << printer->line("") << "i32.const " << entry.offset;
          out << printer->line("") << "i32.const " << entry.length;
        }
      }
    }

    if (actual_params != nullptr && !actual_params->children.empty()) {
      if (actual_params->next_child()->type == Node::eq_op) {
        auto *id = actual_params->next_child()->find_first(Node::id);
        auto *sym = sym_table->lookup(id->value, id->function_name);
        if (sym->is_global()) {
          out << printer->line("global.get") << printer->add_name(sym->name);
        } else {
          out << printer->line("local.get") << printer->add_name(sym->name);
        }
      }
    }

    out << printer->line("call") << printer->add_name(fun_sym->name);
    if (fun_sym->name == "halt") {
      out << printer->line("unreachable");
    }
    break;
  }
  case Node::if_statement:
  case Node::if_else_statement: {
    out << printer->dedent() << printer->line(")");
    break;
  }
  case Node::while_statement: {
    // @HACK: pretty ugly, but whatever
    prev_block_state();
    out << printer->line("br $_loop" + get_block_state());
    out << printer->dedent() << printer->line(")");
    out << printer->dedent() << printer->line(")");
    break;
  }
  case Node::break_statement: {
    out << printer->line("br $_block" + get_block_state());
    break;
  }
  case Node::return_statement: {
    if (!node->children.empty()) {
      if (node->next_child()->type == Node::eq_op) {
        auto return_id = node->next_child()->next_child();
        auto sym =
            sym_table->lookup(return_id->value, return_id->function_name);
        if (sym->is_global()) {
          out << printer->line("global.get") << printer->add_name(sym->name);
        } else {
          out << printer->line("local.get") << printer->add_name(sym->name);
        }
      }
    }
    out << printer->line("return");
    break;
  }
  case Node::eq_op: {
    auto id = node->next_child();
    auto name = id->value;
    auto sym = sym_table->lookup(name, id->function_name);

    // @HACK: a very hacky way to generate nested assignments (i = j = k = 1;)
    // should be handled recursively
    auto *assigned = node->children[1];
    if (assigned != nullptr && assigned->type == Node::eq_op) {
      auto *nested_assigned_id = assigned->next_child();
      auto *nested_assigned_sym = sym_table->lookup(
          nested_assigned_id->value, nested_assigned_id->function_name);

      if (nested_assigned_sym != nullptr) {
        if (nested_assigned_sym->is_global()) {
          out << printer->line("global.get " + nested_assigned_sym->wasm_name);
        } else {
          out << printer->line("local.get " + nested_assigned_sym->wasm_name);
        }
      }
    }

    if (sym != nullptr) {
      if (sym->is_global()) {
        out << printer->line("") << "global.set"
            << printer->add_name(sym->name);
      } else {
        out << printer->line("") << "local.set" << printer->add_name(sym->name);
      }
    }

    break;
  }
  case Node::add_op: {
    out << printer->line("") << "i32.add";
    break;
  }
  case Node::sub_op: {
    if (node->children.size() > 1) {
      out << printer->line("i32.sub");
    }
    break;
  }
  case Node::mul_op: {
    out << printer->line("") << "i32.mul";
    break;
  }
  case Node::div_op: {
    out << printer->line("") << "i32.div_s";
    break;
  }
  case Node::mod_op: {
    out << printer->line("") << "i32.rem_s";
    break;
  }
  case Node::lt_op: {
    out << printer->line("") << "i32.lt_s";
    break;
  }
  case Node::lteq_op: {
    out << printer->line("") << "i32.le_s";
    break;
  }
  case Node::gt_op: {
    out << printer->line("") << "i32.gt_s";
    break;
  }
  case Node::gteq_op: {
    out << printer->line("") << "i32.ge_s";
    break;
  }
  case Node::eqeq_op: {
    out << printer->line("") << "i32.eq";
    break;
  }
  case Node::noteq_op: {
    out << printer->line("") << "i32.ne";
    break;
  }
  case Node::not_op: {
    out << printer->line("") << "i32.const 1";
    out << printer->line("") << "i32.xor";
    break;
  }
  case Node::bin_and_op: {
    auto *fun_sym = sym_table->find_function("__and_op");
    out << printer->line("") << "call" << printer->add_name(fun_sym->name);
    break;
  }
  case Node::bin_or_op: {
    auto *fun_sym = sym_table->find_function("__or_op");
    out << printer->line("") << "call" << printer->add_name(fun_sym->name);
  }
  case Node::int_t:
  case Node::boolean_t: {
    if (node->is_const()) {
      auto type = node->type;
      auto value = node->value;
      if (type == Node::boolean_t) {
        out << printer->line("") << "i32.const"
            << printer->add_bool_const(value == "true");
      } else {
        out << printer->line("") << "i32.const"
            << printer->add_int_const(std::stoi(value));
      }
    }

    break;
  }
  default:
    break;
  }

  auto iter = printer->decorations.find(node);
  if (iter != printer->decorations.end()) {
    if (iter->second.post != nullptr) {
      iter->second.post(node);
    }
  }
}

/**
 * @brief Generate variables for given scope (function or global)
 *
 * @param scope_name name of the scope
 */
void CodeGenerator::generate_vars(std::string scope_name) {
  bool is_global = scope_name == "global";

  auto scope = sym_table->get_scope(scope_name);

  for (auto const &[_, sym] : scope) {
    if (sym->kind == "variable") {
      if (is_global) {
        out << printer->line("(global") << printer->add_name(sym->name)
            << printer->add("(mut i32)") << printer->add("(i32.const 0)")
            << printer->add(")");
      } else {
        // out << printer->indent() << printer->add_local(sym->name)
        //     << printer->dedent();
        out << printer->indent() << printer->line("(local")
            << printer->add_name(sym->name) << printer->add("i32")
            << printer->add(")") << printer->dedent();
      }
    }
  }
}

/**
 * @brief Read runtime functions specified in PROJECT_ROOT/src/lib/runtime.wat
 * and inject them to generated WAT code
 */
void CodeGenerator::inject_runtime() {
  std::ifstream runtime("src/lib/runtime.wat");
  if (runtime.is_open()) {
    std::string line;
    while (std::getline(runtime, line)) {
      out << printer->line(line);
    }
    out << "\n";
  }
}
