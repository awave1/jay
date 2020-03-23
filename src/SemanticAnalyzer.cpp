#include "./include/SemanticAnalyzer.hpp"

std::shared_ptr<ast_node_t> SemanticAnalyzer::get_ast() { return ast; }

bool SemanticAnalyzer::validate() {
  // Pass 1
  this->traverse(this->ast.get(), nullptr,
                 std::bind(&SemanticAnalyzer::globals_post_order_pass, this,
                           std::placeholders::_1));

  std::cout << *sym_table << std::endl;

  // // Pass 2
  this->traverse(this->ast.get(),
                 std::bind(&SemanticAnalyzer::sym_table_pre_post_order_pass,
                           this, std::placeholders::_1),
                 std::bind(&SemanticAnalyzer::sym_table_pre_post_order_pass,
                           this, std::placeholders::_1));

  // // Pass 3
  // this->traverse(this->ast.get(), nullptr,
  // &type_checking_post_order_pass);

  // // Pass 4
  // this->traverse(this->ast.get(), &catch_all_pre_post_order_pass,
  //                &catch_all_pre_post_order_pass);

  return false;
}

void SemanticAnalyzer::traverse(ast_node_t *node,
                                std::function<bool(ast_node_t *n)> pre,
                                std::function<bool(ast_node_t *n)> post) {

  if (pre != nullptr) {
    pre(node);
  }

  for (auto *next : node->children) {
    traverse(next, pre, post);
  }

  if (post != nullptr) {
    post(node);
  }
}

// pass 1
bool SemanticAnalyzer::globals_post_order_pass(ast_node_t *node) {
  // ignore top-level root as it's useless for symbol table
  if (node->type != ast_node_t::Node::program) {
    switch (node->type) {
    case ast_node_t::Node::main_func_decl:
      if (node->children.empty()) {
        return false;
      }

      sym_table->insert(Symbol("main", ast_node_t::Node::void_t), 1);

      break;
    case ast_node_t::Node::global_var_decl:
      if (node->children.empty()) {
        break;
      }

      sym_table->insert(
          Symbol(node->children[1]->value, node->children[0]->type), 1);

      break;
    case ast_node_t::Node::function_decl:
      if (node->children.empty()) {
        break;
      }

      sym_table->insert(
          Symbol(node->children[1]->value, node->children[0]->type), 1);
      break;
    default:
      break;
    }
  }

  return false;
}

// pass 2
bool SemanticAnalyzer::sym_table_pre_post_order_pass(ast_node_t *node) {
  if (node->type != ast_node_t::Node::program) {
    std::cout << "pass 2:\n" << *node << std::endl;
  }
  return false;
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
