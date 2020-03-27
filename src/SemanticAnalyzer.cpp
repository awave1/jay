#include "./include/SemanticAnalyzer.hpp"

std::shared_ptr<ast_node_t> SemanticAnalyzer::get_ast() { return ast; }

bool SemanticAnalyzer::validate() {
  std::vector<ast_node_t *> main =
      ast->find_all(ast_node_t::Node::main_func_decl);

  if (!main.empty()) {
    if (main.size() > 1) {
      std::throw_with_nested(
          std::runtime_error("Multiple `main()` declarations found. Aborting"));
      return false;
    }
    // Pass 1
    this->traverse(ast.get(), nullptr,
                   std::bind(&SemanticAnalyzer::globals_post_order_pass, this,
                             std::placeholders::_1));

    // Pass 2
    // fill out symbol table
    this->traverse(
        ast.get(),
        std::bind(&SemanticAnalyzer::build_scope, this, std::placeholders::_1),
        std::bind(&SemanticAnalyzer::sym_table_pre_post_order_pass, this,
                  std::placeholders::_1));

    // Pass 3
    this->traverse(
        ast.get(),
        std::bind(&SemanticAnalyzer::enter_scope, this, std::placeholders::_1),
        std::bind(&SemanticAnalyzer::type_checking_post_order_pass, this,
                  std::placeholders::_1));

    std::cout << *sym_table << std::endl;

    // Pass 4
  } else {
    std::throw_with_nested(std::runtime_error("No `main()` found. Aborting"));
    return false;
  }

  return true;
}

bool SemanticAnalyzer::traverse(ast_node_t *node,
                                std::function<bool(ast_node_t *n)> pre,
                                std::function<void(ast_node_t *n)> post) {

  if (pre != nullptr && !pre(node)) {
    return false;
  }

  for (auto *next : node->children) {
    if (!traverse(next, pre, post)) {
      return false;
    }
  }

  if (post != nullptr) {
    post(node);
  }

  return true;
}

// pass 1
void SemanticAnalyzer::globals_post_order_pass(ast_node_t *node) {
  switch (node->type) {
  case ast_node_t::Node::main_func_decl: {
    if (node->children.empty()) {
      break;
    }
    sym_table->define(new FunctionSymbol("main", {}, ast_node_t::Node::void_t,
                                         sym_table->current_scope_level,
                                         sym_table->current_scope),
                      "global");

    auto id = node->children[1]->value;
    auto *block = node->find_first(ast_node_t::Node::block);
    auto ids = block->find_recursive(ast_node_t::Node::id);
    for (auto *_id : ids) {
      _id->function_name = id;
    }
    break;
  }
  case ast_node_t::Node::function_decl: {
    if (node->children.empty()) {
      break;
    }

    auto id = node->children[1]->value;
    auto type = node->children[0]->type;
    auto *params = node->find_first(ast_node_t::Node::formal_params);
    auto *block = node->find_first(ast_node_t::Node::block);

    std::vector<Symbol> sym_params;
    if (params != nullptr) {
      for (auto *formal : params->children) {
        auto *formal_id = formal->children[1];
        auto *formal_type = formal->children[0];

        sym_params.push_back(
            Symbol(formal_id->value, "parameter", formal_type->type,
                   sym_table->current_scope_level, sym_table->current_scope));
      }
    }

    // mark if the block supposed to have return statement
    if (block != nullptr) {
      block->is_return_block = type != ast_node_t::Node::void_t;
      auto ids = block->find_recursive(ast_node_t::Node::id);
      std::cout << "ids: " << ids.size() << std::endl;
      for (auto *_id : ids) {
        _id->function_name = id;
        std::cout << *_id << std::endl;
      }
    }

    sym_table->define(new FunctionSymbol(id, sym_params, type,
                                         sym_table->current_scope_level,
                                         sym_table->current_scope),
                      "global");
    break;
  }
  case ast_node_t::Node::global_var_decl:
    if (node->children.empty()) {
      break;
    }

    sym_table->define(new Symbol(node->children[1]->value, "variable",
                                 node->children[0]->type,
                                 sym_table->current_scope_level,
                                 sym_table->current_scope),
                      "global");
    break;
  case ast_node_t::Node::while_statement: {
    auto *block = node->find_first(ast_node_t::Node::block);
    if (block != nullptr) {
      block->is_while_block = true;
    }
    break;
  }
  case ast_node_t::Node::add_op:
  case ast_node_t::Node::sub_op:
  case ast_node_t::Node::mul_op:
  case ast_node_t::Node::div_op:
  case ast_node_t::Node::mod_op:
    node->expected_type = ast_node_t::Node::int_t;
    break;
  default:
    break;
  }
}

// pass 2
void SemanticAnalyzer::sym_table_pre_post_order_pass(ast_node_t *node) {
  if (node->type != ast_node_t::Node::program) {
    switch (node->type) {
    case ast_node_t::Node::variable_decl: {
      if (node->children.empty()) {
        std::throw_with_nested(
            std::runtime_error("Wrong variable declaration"));
        break;
      }

      auto type_node = node->children[0];
      auto id_node = node->children[1];

      if (!is_declaration_allowed()) {
        std::cout << "scope: " << sym_table->current_scope << std::endl;
        std::cerr
            << "Error: `" + get_str_for_type(type_node->type) + " "
            << id_node->value
            << "` A local declaration was not in an outermost block. Line: "
            << node->linenum << std::endl;
        break;
      }

      sym_table->define(new Symbol(id_node->value, "variable", type_node->type,
                                   sym_table->current_scope_level,
                                   sym_table->current_scope),
                        id_node->function_name);
      break;
    }
    case ast_node_t::Node::main_func_decl: {
      auto block_nodes = node->find_recursive(ast_node_t::Node::block);
      for (auto *b : block_nodes) {
        auto *break_node = b->find_first(ast_node_t::Node::break_statement);
        if (!b->is_while_block && break_node != nullptr) {
          std::cerr << "Error: `break` statement can only occur in `while` "
                       "loops. Line: "
                    << break_node->linenum << std::endl;
        }
      }

      break;
    }
    case ast_node_t::Node::function_decl: {
      auto *block = node->find_first(ast_node_t::Node::block);
      auto return_nodes =
          block->find_recursive(ast_node_t::Node::return_statement);
      auto *type = node->children[0];
      auto break_nodes =
          block->find_recursive(ast_node_t::Node::break_statement);

      if (block->is_return_block) {
        if (return_nodes.empty()) {
          std::cerr << "Error: missing `return` statement. Line: "
                    << node->linenum << std::endl;
          break;
        } else {
          for (auto *return_node : return_nodes) {
            if (return_node->children.empty()) {
              std::cerr << "Error: function should return a value. Line: "
                        << return_node->linenum << std::endl;
            } else {
              // return exists, check if value matches function return type
              auto *return_val = return_node->children[0];
              bool is_valid_return = true;
              ast_node_t::Node found_type;

              if (return_val->type == ast_node_t::Node::id) {
                auto *sym = sym_table->lookup(return_val->value,
                                              return_val->function_name);
                found_type = sym->type;
                is_valid_return = sym->type == type->type;
              } else if (return_val->type == ast_node_t::Node::function_call) {
                auto *id = return_val->find_first(ast_node_t::Node::id);
                auto *sym = sym_table->find_function(id->value);
                found_type = sym->type;
                is_valid_return = sym->type == type->type;
              } else if (return_val->is_bool_expr() ||
                         return_val->is_num_expr()) {
                is_valid_return = validate_expr(
                    return_val, expression_types.at(return_val->type));
              } else {
                found_type = return_val->type;
                is_valid_return = return_val->type == type->type;
              }

              if (!is_valid_return) {
                std::cerr << "Error: mismatched return type. Was expecting `"
                          << get_str_for_type(type->type) << "`, but got `"
                          << get_str_for_type(found_type)
                          << "`. Line: " << return_node->linenum << std::endl;
              }
            }
          }
        }
      } else if (type->type == ast_node_t::Node::void_t) {
        for (auto *return_node : return_nodes) {
          if (!return_node->children.empty()) {
            std::cerr << "Error: void function cannot return values. Line: "
                      << return_node->linenum << std::endl;
          }
        }
      }

      auto block_nodes = node->find_recursive(ast_node_t::Node::block);
      for (auto *b : block_nodes) {
        auto *break_node = b->find_first(ast_node_t::Node::break_statement);
        if (!b->is_while_block && break_node != nullptr) {
          std::cerr << "Error: `break` statement can only occur in `while` "
                       "loops. Line: "
                    << break_node->linenum << std::endl;
        }
      }

      sym_table->current_scope--;
      break;
    }
    case ast_node_t::Node::block: {
      for (auto *c : node->children) {
        switch (c->type) {
        case ast_node_t::Node::statement_expr: {
          auto *expression = c->children[0];
          if (expression->type == ast_node_t::Node::function_call) {
            auto *actual_params =
                expression->find_first(ast_node_t::Node::actual_params);

            auto *fun_name = expression->find_first(ast_node_t::Node::id);
            auto *fun_sym = sym_table->find_function(fun_name->value);

            if (fun_name->value == "main") {
              std::cerr
                  << "Error: Cannot call `main()` function directly. Line: "
                  << expression->linenum << std::endl;
              break;
            }

            if (actual_params->children.size() != fun_sym->params.size()) {
              std::cerr
                  << "Error: Mismatched number of actual parameters. Expected: "
                  << fun_sym->params.size() << ", but got "
                  << actual_params->children.size()
                  << ". Line: " << expression->linenum << std::endl;
            }
          } else if (expression->type == ast_node_t::Node::eq_op) {
            auto *id = expression->children[0];
            if (sym_table->lookup(id->value, id->function_name) == nullptr) {
              std::cerr << "Error: undefined identifier `" << id->value
                        << "`. Line: " << expression->linenum << std::endl;
            }
          }
        }
        default:
          break;
        }
      }

      sym_table->exit_scope();
      break;
    }
    default:
      break;
    }
  }
}

// pass 3
void SemanticAnalyzer::type_checking_post_order_pass(ast_node_t *node) {
  switch (node->type) {
  case ast_node_t::Node::main_func_decl:
  case ast_node_t::Node::function_decl:
    sym_table->current_scope--;
    break;
  case ast_node_t::Node::id: {
    if (sym_table->lookup(node->value, node->function_name) == nullptr) {
      std::cerr << "Error: unknown identifier `" << node->value
                << "`. Line: " << node->linenum << std::endl;
    }
    break;
  }
  case ast_node_t::Node::if_statement:
  case ast_node_t::Node::while_statement:
  case ast_node_t::Node::if_else_statement: {
    auto *expr = node->children[0];
    if (expr->is_bool_expr()) {
      auto it = expression_types.find(expr->type);
      if (it == expression_types.end()) {
        std::cerr
            << "Error: `if` expression expects a boolean expression, found: `"
            << expr->value << "`. Line: " << expr->linenum << std::endl;
        break;
      }

      auto expected_types = expression_types.at(expr->type);

      bool is_bool = true;
      if (expr->children.size() == 2) {
        // check binary expression

        auto *l = expr->children[0];
        auto *r = expr->children[1];

        is_bool = validate_expr(l, expected_types) &&
                  validate_expr(r, expected_types);

      } else {
        // check unary expression
        auto *r = expr->children[0];
        auto r1 = expected_types[0];
        ast_node_t::Node type;

        if (r->type == ast_node_t::Node::id) {
          type = sym_table->lookup(r->value, r->function_name)->type;
        } else if (r->type == ast_node_t::Node::function_call) {
          auto *id = r->find_first(ast_node_t::Node::id);
          auto *fun_symbol = sym_table->find_function(id->value);
          type = fun_symbol->type;
        } else if (r->is_num_expr()) {
          type = r->expected_type;
        }

        is_bool = type == r1[0];
      }

      if (!is_bool) {
        std::cerr << "Error: Invalid boolean expression used in `"
                  << get_str_for_type(node->type)
                  << "` expression. Line: " << expr->linenum << std::endl;
        break;
      }
    } else if (expr->type == ast_node_t::Node::function_call) {
      auto *id = expr->find_first(ast_node_t::Node::id);

      if (id == nullptr) {
        break;
      }

      auto *fun_sym = sym_table->find_function(id->value);
      if (fun_sym->type != ast_node_t::Node::boolean_t) {
        std::cerr
            << "Error: function must have `boolean` return type to be used in `"
            << get_str_for_type(node->type) << "` expression, found `"
            << get_str_for_type(fun_sym->type) << "`. Line: " << expr->linenum
            << std::endl;
        break;
      }
    } else if (expr->type == ast_node_t::Node::id) {
      // just a var, look it up in symbol table and find its type
      // at this point it should exist

      auto id_symbol = sym_table->lookup(expr->value, expr->function_name);
      if (id_symbol->type != ast_node_t::Node::boolean_t) {
        std::cerr << "Error: identifier `" << expr->value
                  << "` must have `boolean` type, found `"
                  << get_str_for_type(id_symbol->type)
                  << "`. Line: " << expr->linenum << std::endl;
        break;
      }
    } else if (expr->is_num_expr()) {
      std::cerr << "Error: Invalid boolean expression used in `"
                << get_str_for_type(node->type)
                << "` expression. Line: " << expr->linenum << std::endl;
      break;
    }

    break;
  }
  case ast_node_t::Node::eq_op: {
    auto *id = node->children[0];
    auto *assigned = node->children[1];
    auto *sym = sym_table->lookup(id->value, id->function_name);
    ast_node_t::Node found_type;

    bool types_match = true;

    if (assigned->children.empty()) {
      // if it's just a single value OR an id
      if (assigned->type == ast_node_t::Node::id) {
        found_type =
            sym_table->lookup(assigned->value, assigned->function_name)->type;
      } else {
        found_type = assigned->type;
      }
    } else {
      // if it's something else (expression or function call)
      if (assigned->type == ast_node_t::Node::function_call) {
        auto *id = assigned->find_first(ast_node_t::Node::id);
        auto *fun_sym = sym_table->find_function(id->value);
        found_type = fun_sym->type;
      } else if (assigned->is_num_expr()) {
        found_type = assigned->expected_type;
      }
    }

    types_match = sym->type == found_type;
    if (!types_match) {
      std::cerr << "Error: unexpected assignment to variable `" << id->value
                << "` of type `" << get_str_for_type(sym->type)
                << "`, found type `" << get_str_for_type(found_type)
                << "`. Line: " << node->linenum << std::endl;
      break;
    }

    break;
  }
  case ast_node_t::Node::add_op:
  case ast_node_t::Node::sub_op:
  case ast_node_t::Node::mul_op:
  case ast_node_t::Node::div_op:
  case ast_node_t::Node::mod_op: {
    auto expected_types = expression_types.at(node->type);
    bool is_valid_expr = true;
    std::cout << *node << std::endl;
    std::cout << node->children.size() << std::endl;
    if (node->children.size() == 2) {
      auto *left = node->children[0];
      auto *right = node->children[1];

      is_valid_expr = validate_expr(left, expected_types) &&
                      validate_expr(right, expected_types);
    } else {
      // it's a unary minus
      auto *right = node->children[0];
      is_valid_expr = validate_expr(right, expected_types);
    }

    if (!is_valid_expr) {
      std::cerr << "Error: mismatched types in arithmetic expression. Line: "
                << node->linenum << std::endl;
      break;
    }

    break;
  }
  default:
    break;
  }
}

// pass 4
bool SemanticAnalyzer::catch_all_pre_post_order_pass(ast_node_t *node) {
  return false;
}

bool SemanticAnalyzer::build_scope(ast_node_t *node) {
  switch (node->type) {
  case ast_node_t::Node::main_func_decl:
    sym_table->push_scope("main");
  case ast_node_t::Node::function_decl: {
    auto *id = node->find_first(ast_node_t::Node::id);
    sym_table->push_scope(id->value);
    break;
  }
  case ast_node_t::Node::block:
    sym_table->enter_scope();
    break;
  default:
    break;
  }
  return true;
}

bool SemanticAnalyzer::enter_scope(ast_node_t *node) {
  switch (node->type) {
  case ast_node_t::Node::main_func_decl:
  case ast_node_t::Node::function_decl:
    sym_table->current_scope++;
    break;
  default:
    break;
  }

  return true;
}

bool SemanticAnalyzer::is_declaration_allowed() {
  auto scope = sym_table->current_scope_level;
  // declaration allowed only at levels 1 and 2
  return scope == 1 || scope == 2;
}

bool SemanticAnalyzer::validate_expr(ast_node_t *l,
                                     expr_list_t expected_types) {
  ast_node_t::Node l_type;

  std::cout << "validating: \n" << *l << std::endl;
  std::cout << "exprected types bool or int " << expected_types.size()
            << std::endl;

  if (l->type == ast_node_t::Node::id) {
    auto sym = sym_table->lookup(l->value, l->function_name);
    l_type = sym->type;
  } else if (l->type == ast_node_t::Node::function_call) {
    auto id = l->find_first(ast_node_t::Node::id);
    auto sym = sym_table->find_function(id->value);
    l_type = sym->type;
  } else if (l->is_bool_expr() || l->is_num_expr()) {
    auto expected = expression_types.at(l->type);
    if (l->children.size() == 2) {
      return validate_expr(l->children[0], expected) &&
             validate_expr(l->children[1], expected);
    } else {
      return validate_expr(l->children[0], expected);
    }
  } else {
    l_type = l->type;
  }

  if (expected_types.size() == 2) {
    auto r1 = expected_types[0];
    auto r2 = expected_types[1];
    return l_type == r1[0] || l_type == r2[0];
  } else {
    auto r1 = expected_types[0];
    return l_type == r1[0];
  }
}
