#include "./include/JayCompiler.hpp"
#include "./include/SemanticAnalyzer.hpp"
#include <fstream>
#include <iostream>
#include <memory>

/**
 * @brief build an ast from bison generated parser
 *
 * @param driver main driver, contains lexer and parser
 * @param is input stream from where parser will be reading input
 * @param file filename, if input stream is a fstream
 */
void build_ast(yy::JayCompiler &driver, std::istream *is, std::string file) {
  std::shared_ptr<ast_node_t> ast(driver.parse(is, file));

  if (ast) {
    std::shared_ptr<SymTable> sym_table(new SymTable());
    std::unique_ptr<SemanticAnalyzer> semantic_analyzer(
        new SemanticAnalyzer(ast, sym_table));

    std::cout << *ast << std::endl;

    bool is_valid = semantic_analyzer->validate();
    if (is_valid) {
      std::cout << *ast << std::endl;
    } else {
      std::cerr << "Failed semantic checking" << std::endl;
      exit(1);
    }
  } else {
    std::cerr << "Failed parsing" << std::endl;
    exit(1);
  }
}

int main(int argc, char **argv) {
  yy::JayCompiler driver;

  if (argc >= 2) {
    std::string filename = argv[1];
    std::ifstream file{filename};
    if (!file.is_open()) {
      return 1;
    } else {
      build_ast(driver, &file, filename);
    }
  } else {
    std::cerr << "please specify the filename" << std::endl;
  }

  return 0;
}