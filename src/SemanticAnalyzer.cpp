/**
 * @file SemanticAnalyzer.cpp
 * @author Artem Golovin (30018900)
 * @brief Perform semantic analysis of the parsed AST and build a symbol table
 */

#include "SemanticAnalyzer.hpp"

/**
 * @brief perform semantic validation of the ast and build a symtable. the
 * method traverses ast multiple times to find all semantic errors
 *
 * @return true if ast is semantically correct and passed all the checks
 * @return false otherwise
 */
bool SemanticAnalyzer::validate() {
  std::vector<ASTNode *> main = ast->find_all(Node::main_func_decl);

  if (!main.empty()) {
    if (main.size() > 1) {
      semantic_error("Multiple `main()` declarations found. Aborting");
      return false;
    }

    // Pass 1
    std::vector<bool> glob_error_stack;
    this->traverse(ast.get(), nullptr,
                   std::bind(&SemanticAnalyzer::globals_post_order_pass_cb,
                             this, std::placeholders::_1,
                             std::placeholders::_2),
                   glob_error_stack);
    bool global_pass = glob_error_stack.empty();

    // Pass 2
    // fill out symbol table
    std::vector<bool> sym_table_error_stack;
    this->traverse(ast.get(),
                   std::bind(&SemanticAnalyzer::build_scope_cb, this,
                             std::placeholders::_1, std::placeholders::_2),
                   std::bind(&SemanticAnalyzer::sym_table_post_pass_cb, this,
                             std::placeholders::_1, std::placeholders::_2),
                   sym_table_error_stack);
    bool sym_table_pass = sym_table_error_stack.empty();

    // Pass 3
    std::vector<bool> type_checking_err_stack;
    this->traverse(
        ast.get(),
        std::bind(&SemanticAnalyzer::enter_scope_cb, this,
                  std::placeholders::_1, std::placeholders::_2),
        std::bind(&SemanticAnalyzer::type_checking_post_order_pass_cb, this,
                  std::placeholders::_1, std::placeholders::_2),
        type_checking_err_stack);
    bool type_checking_pass = type_checking_err_stack.empty();

    return global_pass && sym_table_pass && type_checking_pass;
  } else {
    semantic_error("No `main()` found. Aborting");
    return false;
  }

  return false;
}

/**
 * @brief traverse the ast with possible pre and post order callbacks.
 *
 * @param node ast node to start traversing from
 * @param pre pre-order callback function
 * @param post pos-order callback function
 * @param err_stack a vector to keep track of errors displayed. Rather
 * **hacky** way to allow semantic analyzer to print all errors after checks
 * have been performed
 */
void SemanticAnalyzer::traverse(
    ASTNode *node,
    std::function<void(ASTNode *n, std::vector<bool> &err_stack)> pre,
    std::function<void(ASTNode *n, std::vector<bool> &err_stack)> post,
    std::vector<bool> &err_stack) {

  if (pre != nullptr) {
    pre(node, err_stack);
  }

  for (auto *next : node->children) {
    traverse(next, pre, post, err_stack);
  }

  if (post != nullptr) {
    post(node, err_stack);
  }
}

/**
 * @brief post-order callback function, used to collect & populate sym table
 * with information about global variables and functions. In addition it also
 * creates scopes for all global functions and specifies allowed function
 * scope for all identifiers found in the function block.
 *
 * @param node ast node returned from `#traverse`
 * @param err_stack error stack returned from `#traverse`
 */
void SemanticAnalyzer::globals_post_order_pass_cb(
    ASTNode *node, std::vector<bool> &err_stack) {
  switch (node->type) {
  case Node::main_func_decl:
  case Node::function_decl: {
    // TODO: ensure `main` doesn't have args
    if (node->children.empty()) {
      break;
    }

    auto id = node->children[1]->value;
    auto type = node->children[0]->type;
    auto *params = node->find_first(Node::formal_params);
    auto *block = node->find_first(Node::block);

    sym_table->push_scope(id);

    std::vector<Symbol> sym_params;
    if (params != nullptr) {
      auto ids = params->find_recursive(Node::id);

      for (auto *formal : params->children) {
        auto *formal_id = formal->children[1];
        auto *formal_type = formal->children[0];
        auto *param_sym = new Symbol(
            formal_id->value, "parameter", formal_type->type,
            sym_table->current_scope_level + 1, sym_table->current_scope);

        sym_params.push_back(*param_sym);

        sym_table->define(param_sym, id);
      }

      for (auto *_id : ids) {
        _id->function_name = id;
        _id->is_formal_param = true;
      }
    }

    // mark if the block supposed to have return statement
    if (block != nullptr) {
      block->is_return_block = type != Node::void_t;
      auto ids = block->find_recursive(Node::id);

      // assign function name to all ids inside the function block
      for (auto *_id : ids) {
        _id->function_name = id;
      }
    }

    sym_table->define(new FunctionSymbol(id, sym_params, type,
                                         sym_table->current_scope_level,
                                         sym_table->current_scope),
                      "global");
    break;
  }
  case Node::global_var_decl: {
    if (node->children.empty()) {
      break;
    }

    sym_table->define(new Symbol(node->children[1]->value, "variable",
                                 node->children[0]->type,
                                 sym_table->current_scope_level,
                                 sym_table->current_scope),
                      "global");
    break;
  }
  case Node::while_statement: {
    auto blocks = node->find_recursive(Node::block);
    for (auto *block : blocks) {
      block->is_while_block = true;
    }

    break;
  }
  case Node::add_op:
  case Node::sub_op:
  case Node::mul_op:
  case Node::div_op:
  case Node::mod_op: {
    node->expected_type = Node::int_t;

    // @HACK: indicate that a number is negative (it *should* be done in parser)
    if (node->type == Node::sub_op && node->children.size() == 1 &&
        node->next_child()->type == Node::int_t) {
      node->next_child()->value = "-" + node->next_child()->value;
    }
    break;
  }
  default:
    break;
  }
}

/**
 * @brief post-order callback function, used to populate the symtable. In
 * addition it performs lookups to make sure identifiers haven't been
 * re-declared multiple times or if they exist.
 *
 * @param node ast node returned from `#traverse`
 * @param err_stack error stack returned from `#traverse`
 */
void SemanticAnalyzer::sym_table_post_pass_cb(ASTNode *node,
                                              std::vector<bool> &err_stack) {
  if (node->type != Node::program) {
    switch (node->type) {
    case Node::variable_decl: {
      if (node->children.empty()) {
        std::throw_with_nested(
            std::runtime_error("Wrong variable declaration"));
        break;
      }

      auto type_node = node->children[0];
      auto id_node = node->children[1];

      if (!is_declaration_allowed()) {
        semantic_error(
            "`" + get_str_for_type(type_node->type) + " " + id_node->value +
                "` A local declaration was not in an outermost block.",
            node->linenum);
        err_stack.push_back(false);
        break;
      } else if (sym_table->lookup_in_local(
                     id_node->value, id_node->function_name) != nullptr) {
        semantic_error(
            "`" + get_str_for_type(type_node->type) + " " + id_node->value +
                "`: Duplicated declaration of variables is not allowed.",
            node->linenum);
        err_stack.push_back(false);
        break;
      }

      sym_table->define(new Symbol(id_node->value, "variable", type_node->type,
                                   sym_table->current_scope_level,
                                   sym_table->current_scope),
                        id_node->function_name);
      break;
    }
    case Node::main_func_decl:
    case Node::function_decl: {
      auto *block = node->find_first(Node::block);
      auto return_nodes = block->find_recursive(Node::return_statement);
      auto *id = node->find_first(Node::id);
      auto *type = node->children[0];
      auto break_nodes = block->find_recursive(Node::break_statement);

      if (block->is_return_block) {
        if (return_nodes.empty()) {
          semantic_error("Missing `return` statement.", node->linenum);
          err_stack.push_back(false);
          break;
        } else {
          for (auto *return_node : return_nodes) {
            if (return_node->children.empty()) {
              semantic_error("Function should return a value.",
                             return_node->linenum);
              err_stack.push_back(false);
              break;
            } else {
              // return exists, check if value matches function return type
              auto *return_val = return_node->children[0];
              bool is_valid_return = true;
              Node found_type;

              if (return_val->type == Node::id) {
                auto *sym = sym_table->lookup(return_val->value,
                                              return_val->function_name);
                found_type = sym->type;
                is_valid_return = sym->type == type->type;
              } else if (return_val->type == Node::function_call) {
                auto *id = return_val->find_first(Node::id);
                auto *sym = sym_table->find_function(id->value);
                found_type = sym->type;
                is_valid_return = sym->type == type->type;
              } else if (return_val->type == Node::eq_op) {
                auto *lhs = return_val->next_child();
                auto *lhs_sym =
                    sym_table->lookup(lhs->value, lhs->function_name);
                found_type = lhs_sym->type;
                is_valid_return = validate_expr(
                    return_val, expression_types.at(return_val->type));
              } else if (return_val->is_bool_expr() ||
                         return_val->is_num_expr()) {
                if (return_val->is_bool_expr()) {
                  found_type = Node::boolean_t;
                } else if (return_val->is_num_expr()) {
                  found_type = Node::int_t;
                }

                is_valid_return = validate_expr(
                    return_val, expression_types.at(return_val->type));
              } else {
                found_type = return_val->type;
                is_valid_return = return_val->type == type->type;
              }

              if (!is_valid_return) {
                semantic_error("Mismatched return type. Was expecting `" +
                                   get_str_for_type(type->type) +
                                   "`, but got `" +
                                   get_str_for_type(found_type) + "`",
                               return_node->linenum);
                err_stack.push_back(false);
                break;
              }
            }
          }
        }
      } else if (type->type == Node::void_t) {
        bool found_return_val_in_void = false;
        bool found_return_in_main = false;
        for (auto *return_node : return_nodes) {
          if (id->value == "main") {
            semantic_error("`main()` cannot have `return` statements.",
                           return_node->linenum);
            err_stack.push_back(false);
            found_return_in_main = true;
          }

          if (!return_node->children.empty()) {
            semantic_error("void function `" + id->value +
                               "` cannot return values.",
                           return_node->linenum);
            err_stack.push_back(false);
            found_return_val_in_void = true;
          }
        }

        if (found_return_in_main || found_return_val_in_void) {
          break;
        }
      }

      auto block_nodes = node->find_recursive(Node::block);
      for (auto *b : block_nodes) {
        auto *break_node = b->find_first(Node::break_statement);
        if (!b->is_while_block && break_node != nullptr) {
          semantic_error("`break` statement can only occur in `while` "
                         "loops.",
                         break_node->linenum);
          err_stack.push_back(false);
        }
      }

      sym_table->current_scope--;
      break;
    }
    case Node::block: {
      for (auto *c : node->children) {
        switch (c->type) {
        case Node::statement_expr: {
          auto *expression = c->children[0];
          if (expression->type == Node::function_call) {
            auto *actual_params = expression->find_first(Node::actual_params);

            auto *fun_name = expression->find_first(Node::id);
            auto *fun_sym = sym_table->find_function(fun_name->value);

            if (fun_name->value == "main") {
              semantic_error("Cannot call `main()` function directly.",
                             expression->linenum);
              err_stack.push_back(false);
              break;
            }

            if (actual_params->children.size() != fun_sym->params.size()) {
              semantic_error(
                  "Error: Mismatched number of actual parameters, expected: " +
                      std::to_string(fun_sym->params.size()) + ", but got " +
                      std::to_string(actual_params->children.size()),
                  expression->linenum);
              err_stack.push_back(false);
              break;
            }
          } else if (expression->type == Node::eq_op) {
            auto *id = expression->children[0];
            if (sym_table->lookup(id->value, id->function_name) == nullptr) {
              semantic_error("Undefined identifier `" + id->value + "`.",
                             expression->linenum);
              err_stack.push_back(false);
              break;
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

/**
 * @brief post-order callback function, used to perform type checking of
 * functions, identifiers, expressions, etc.
 *
 * @param node ast node returned from `#traverse`
 * @param err_stack error stack returned from `#traverse`
 */
void SemanticAnalyzer::type_checking_post_order_pass_cb(
    ASTNode *node, std::vector<bool> &err_stack) {
  switch (node->type) {
  case Node::main_func_decl:
  case Node::function_decl:
    sym_table->current_scope--;
    break;
  case Node::id: {
    if (sym_table->lookup(node->value, node->function_name) == nullptr) {
      semantic_error("Unknown identifier `" + node->value + "`.",
                     node->linenum);
      err_stack.push_back(false);
    }
    break;
  }
  case Node::function_call: {
    auto *id = node->find_first(Node::id);
    auto *actual_params = node->find_first(Node::actual_params);
    auto *fun_sym = sym_table->find_function(id->value);

    id->is_function_id = true;

    if (actual_params->children.size() == fun_sym->params.size()) {
      auto children = actual_params->children;
      bool found_mismatch = false;
      for (std::size_t i = 0; i < children.size(); i++) {
        auto *param_node = children[i];
        auto expected_sym = fun_sym->params[i];
        Node found_type;

        if (param_node->type == Node::function_call) {
          // lookup function return type
          auto *id = param_node->find_first(Node::id);
          auto *fun_sym = sym_table->find_function(id->value);
          found_type = fun_sym->type;
        } else if (param_node->type == Node::id) {
          auto *sym =
              sym_table->lookup(param_node->value, param_node->function_name);
          found_type = sym->type;

          param_node->can_generate_wasm_getter = true;
        } else if (param_node->is_bool_expr()) {
          found_type = Node::boolean_t;
        } else if (param_node->is_num_expr()) {
          found_type = Node::int_t;
        } else if (param_node->type == Node::eq_op) {
          auto ids = param_node->find_recursive(Node::id);
          for (auto *id : ids) {
            id->can_generate_wasm_getter = false;
          }

          if (validate_expr(param_node,
                            expression_types.at(param_node->type))) {
            // lhs operand
            auto *res_id = param_node->next_child();
            auto *sym = sym_table->lookup(res_id->value, res_id->function_name);
            found_type = sym->type;
          }
        } else {
          found_type = param_node->type;
        }

        if (found_type != expected_sym.type) {
          semantic_error(
              "Mismatched arguments for function `" + id->value +
                  "`, expected `" + get_str_for_type(expected_sym.type) +
                  "`, but found `" + get_str_for_type(found_type) + "`.",
              node->linenum);
          found_mismatch = true;
        }
      }

      if (found_mismatch) {
        err_stack.push_back(false);
        break;
      }
    }
    break;
  }
  case Node::return_statement: {
    bool is_eq = node->next_child()->type == Node::eq_op;
    if (is_eq) {
      auto ids = node->find_recursive(Node::id);
      for (auto *id : ids) {
        auto *sym = sym_table->lookup(id->value, id->function_name);
        id->can_generate_wasm_getter =
            sym != nullptr && sym->kind != "function";
      }
    } else {
      node->next_child()->can_generate_wasm_getter = true;
    }
    break;
  }
  case Node::if_statement:
  case Node::while_statement:
  case Node::if_else_statement: {
    auto *expr = node->children[0];
    if (expr->is_bool_expr()) {
      auto it = expression_types.find(expr->type);
      if (it == expression_types.end()) {
        semantic_error(
            "`" + get_str_for_type(node->type) +
                "` expression expects a boolean expression, found: `" +
                expr->value,
            node->linenum);
        err_stack.push_back(false);
        break;
      }

      auto ids = expr->find_recursive(Node::id);
      for (auto *id : ids) {
        auto sym = sym_table->lookup(id->value, id->function_name);
        if (sym->kind != "function") {

          id->can_generate_wasm_getter = true;
        }
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
        Node type;

        if (r->type == Node::id) {
          type = sym_table->lookup(r->value, r->function_name)->type;
        } else if (r->type == Node::function_call) {
          auto *id = r->find_first(Node::id);
          auto *fun_symbol = sym_table->find_function(id->value);
          type = fun_symbol->type;
        } else if (r->is_num_expr()) {
          type = r->expected_type;
        }

        is_bool = type == r1[0];
      }

      if (!is_bool) {
        semantic_error("Invalid boolean expression used in `" +
                           get_str_for_type(node->type) + "` expression.",
                       expr->linenum);
        err_stack.push_back(false);
        break;
      }
    } else if (expr->type == Node::function_call) {
      auto *id = expr->find_first(Node::id);

      if (id == nullptr) {
        break;
      }

      auto *fun_sym = sym_table->find_function(id->value);
      if (fun_sym->type != Node::boolean_t) {
        semantic_error(
            "Function must have `boolean` return type to be used in `" +
                get_str_for_type(node->type) + "` expression, found `" +
                get_str_for_type(fun_sym->type),
            expr->linenum);

        err_stack.push_back(false);
        break;
      }
    } else if (expr->type == Node::id) {
      // just a var, look it up in symbol table and find its type
      // at this point it should exist
      auto id_symbol = sym_table->lookup(expr->value, expr->function_name);
      expr->can_generate_wasm_getter = true;

      if (id_symbol->type != Node::boolean_t) {
        semantic_error("Identifier `" + expr->value +
                           "` must have `boolean` type, found `" +
                           get_str_for_type(id_symbol->type) + "`",
                       expr->linenum);
        err_stack.push_back(false);
        break;
      }
    } else if (expr->is_num_expr() || expr->type != Node::boolean_t) {
      semantic_error("Invalid boolean expression used in `" +
                         get_str_for_type(node->type) + "` expression.",
                     expr->linenum);
      err_stack.push_back(false);
      break;
    }

    break;
  }
  case Node::eq_op: {
    auto *id = node->children[0];
    auto *assigned = node->children[1];
    auto *sym = sym_table->lookup(id->value, id->function_name);
    Node found_type;

    bool types_match = true;

    if (assigned->children.empty()) {
      // if it's just a single value OR an id
      if (assigned->type == Node::id) {
        found_type =
            sym_table->lookup(assigned->value, assigned->function_name)->type;
        assigned->can_generate_wasm_getter = true;
      } else {
        found_type = assigned->type;
      }
    } else {
      // if it's something else (expression or function call)
      if (assigned->type == Node::eq_op) {
        auto *id = assigned->next_child();
        auto *sym = sym_table->lookup(id->value, id->function_name);
        found_type = sym->type;
      } else if (assigned->type == Node::function_call) {
        auto *id = assigned->find_first(Node::id);
        auto *fun_sym = sym_table->find_function(id->value);
        found_type = fun_sym->type;
      } else if (assigned->is_num_expr()) {
        found_type = Node::int_t;
      } else if (assigned->is_bool_expr()) {
        found_type = Node::boolean_t;
      }
    }

    if (sym != nullptr) {
      types_match = sym->type == found_type;
      if (!types_match) {
        semantic_error("unexpected assignment to variable `" + id->value +
                           "` of type `" + get_str_for_type(sym->type) +
                           "`, found type `" + get_str_for_type(found_type),
                       node->linenum);
        err_stack.push_back(false);
        break;
      }
    }

    break;
  }
  case Node::not_op:
  case Node::eqeq_op:
  case Node::noteq_op:
  case Node::bin_or_op:
  case Node::bin_and_op:
  case Node::add_op:
  case Node::sub_op:
  case Node::mul_op:
  case Node::div_op:
  case Node::mod_op: {
    auto expected_types = expression_types.at(node->type);
    bool is_valid_expr = true;
    auto ids = node->find_recursive(Node::id);
    for (auto *id : ids) {
      auto sym = sym_table->lookup(id->value, id->function_name);
      if (sym->kind != "function") {

        id->can_generate_wasm_getter = true;
      }
    }

    if (node->children.size() == 2) {
      auto *left = node->children[0];
      auto *right = node->children[1];

      is_valid_expr = validate_expr(left, expected_types) &&
                      validate_expr(right, expected_types);
    } else {
      // it's a unary expressio
      auto *right = node->children[0];
      is_valid_expr = validate_expr(right, expected_types);
    }

    if (!is_valid_expr) {
      semantic_error("Mismatched types in expression.", node->linenum);
      err_stack.push_back(false);
      break;
    }
    break;
  }
  default:
    break;
  }
}

/**
 * @brief pre-order callback function, used to determine block nesting level.
 *
 * @param node ast node returned from `#traverse`
 * @param err_stack error stack returned from `#traverse`
 */
void SemanticAnalyzer::enter_scope_cb(ASTNode *node,
                                      std::vector<bool> &err_stack) {
  switch (node->type) {
  case Node::main_func_decl:
  case Node::function_decl:
    sym_table->current_scope++;
    break;
  default:
    break;
  }
}

/**
 * @brief pre-order callback function, used to enter function scopes
 *
 * @param node ast node returned from `#traverse`
 * @param err_stack error stack returned from `#traverse`
 */
void SemanticAnalyzer::build_scope_cb(ASTNode *node,
                                      std::vector<bool> &err_stack) {
  switch (node->type) {
  case Node::main_func_decl:
    sym_table->push_scope("main");
    break;
  case Node::block:
    sym_table->enter_scope();
    break;
  default:
    break;
  }
}

/**
 * @brief check if declaration is allowed at current block level. the method
 * gets the current block level from the symbol table and verifies allowed
 * block leves. Allowed block levels can be global (1) or local (2). If it's
 * greater than 2, then return false
 *
 * @return true, if current block level allows to declare variables
 * @return false, if block is too deep, and therefore declaration is not
 * allowed
 */
bool SemanticAnalyzer::is_declaration_allowed() {
  auto scope = sym_table->current_scope_level;
  // declaration allowed only at levels 1 and 2
  return scope == 1 || scope == 2;
}

/**
 * @brief perform recursive expression validation based on expected types,
 * predefined in constructor
 *
 * @param expr expression to check
 * @param expected_tyes types expect from the expression
 * @return true if expression is valid
 * @return false otherwise
 */
bool SemanticAnalyzer::validate_expr(ASTNode *l, expr_list_t expected_types) {
  Node l_type;

  if (l->type == Node::id) {
    auto sym = sym_table->lookup(l->value, l->function_name);
    l_type = sym->type;
    // @HACK: why was this here??!
    // l->can_generate_wasm_getter = true;
  } else if (l->type == Node::function_call) {
    auto id = l->find_first(Node::id);
    auto sym = sym_table->find_function(id->value);
    l_type = sym->type;
  } else if (l->type == Node::eq_op) {
    auto expected = expression_types.at(l->type);
    return validate_expr(l->children[0], expected) &&
           validate_expr(l->children[1], expected);
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

/**
 * @brief print semantic error message to stderr
 *
 * @param msg message to display
 * @param linenum line number where the message ocurred
 */
void SemanticAnalyzer::semantic_error(std::string msg, int linenum) {
  auto bold_red_start = "\033[1;31m";
  auto bold_red_end = "\033[0m";
  std::cerr << bold_red_start << "Error: " << msg;
  if (linenum != -1) {
    std::cerr << " Line: " << linenum << bold_red_end << std::endl;
  } else {
    std::cerr << bold_red_end << std::endl;
  }
}

/**
 * @brief print semantic error message to stderr. used when linenumber isn't
 * needed/can't be found
 *
 * @param msg message to display
 */
void SemanticAnalyzer::semantic_error(std::string msg) {
  semantic_error(msg, -1);
}
