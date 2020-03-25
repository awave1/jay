#include "./include/SemanticAnalyzer.hpp"

std::shared_ptr<ast_node_t> SemanticAnalyzer::get_ast() { return ast; }

bool SemanticAnalyzer::validate() {
  auto main = ast->find_recursive(ast_node_t::Node::main_func_decl);
  if (!main.empty()) {
    if (main.size() > 1) {
      std::throw_with_nested(
          std::runtime_error("Multiple `main()` declarations found. Aborting"));
      return false;
    }
    // Pass 1
    this->traverse(this->ast.get(), nullptr,
                   std::bind(&SemanticAnalyzer::globals_post_order_pass, this,
                             std::placeholders::_1));

    // Pass 2
    this->traverse(
        this->ast.get(),
        std::bind(&SemanticAnalyzer::build_scope, this, std::placeholders::_1),
        std::bind(&SemanticAnalyzer::sym_table_pre_post_order_pass, this,
                  std::placeholders::_1));

    std::cout << *sym_table << std::endl;

    // Pass 3

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
  std::cout << "pass1" << std::endl << *node << std::endl;

  switch (node->type) {
  case ast_node_t::Node::main_func_decl:
    if (node->children.empty()) {
      // TODO: Error here?
      break;
    }

    sym_table->define(new FunctionSymbol("main", {}, ast_node_t::Node::void_t));

    break;

  case ast_node_t::Node::function_decl: {
    if (node->children.empty()) {
      break;
    }

    auto id = node->children[1]->value;
    auto type = node->children[0]->type;
    auto *params = node->find_first(ast_node_t::Node::formal_params);

    std::vector<Symbol> sym_params;
    if (params != nullptr) {
      for (auto *formal : params->children) {
        auto *formal_id = formal->children[1];
        auto *formal_type = formal->children[0];

        sym_params.push_back(
            Symbol(formal_id->value, "parameter", formal_type->type));
      }
    }

    sym_table->define(new FunctionSymbol(id, sym_params, type));
    break;
  }
  case ast_node_t::Node::global_var_decl:
    if (node->children.empty()) {
      break;
    }

    sym_table->define(new Symbol(node->children[1]->value, "variable",
                                 node->children[0]->type));

    break;
  default:
    break;
  }
}

// pass 2
void SemanticAnalyzer::sym_table_pre_post_order_pass(ast_node_t *node) {
  std::cout << "pass2" << std::endl << *node << std::endl;
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

      if (!sym_table->has(id_node->value)) {
        sym_table->define(
            new Symbol(id_node->value, "variable", type_node->type));
      } else {
        std::throw_with_nested(
            std::runtime_error("'" + get_str_for_type(type_node->type) + " " +
                               id_node->value + "' has already been defined!"));
      }

      break;
    }
    default:
      break;
    }
  }
}

// pass 3
bool SemanticAnalyzer::type_checking_post_order_pass(ast_node_t *node) {
  std::cout << "pass 3: globals post order pass\n" << *node << std::endl;
  return false;
}

// pass 4
bool SemanticAnalyzer::catch_all_pre_post_order_pass(ast_node_t *node) {
  std::cout << "pass 4: globals post order pass\n" << *node << std::endl;
  return false;
}

bool SemanticAnalyzer::build_scope(ast_node_t *node) {
  if (node->type != ast_node_t::Node::program &&
      node->type == ast_node_t::Node::block) {
    sym_table->push_scope();
  }
  return true;
}
