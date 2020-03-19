#include "./include/SemanticAnalyzer.hpp"

std::shared_ptr<ast_node_t> SemanticAnalyzer::get_ast() { return ast; }

bool SemanticAnalyzer::validate() {
  return ast->find_recursive(ast_node_t::Node::main_func_decl).size() == 1;
}