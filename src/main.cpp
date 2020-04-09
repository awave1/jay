#include "CodeGenerator.hpp"
#include "JayCompiler.hpp"
#include "SemanticAnalyzer.hpp"
#include <cstdlib>
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
  std::shared_ptr<ASTNode> ast(driver.parse(is, file));

  if (ast == nullptr) {
    std::cerr << "Failed parsing" << std::endl;
    exit(EXIT_FAILURE);
  }

  std::shared_ptr<SymTable> sym_table(new SymTable());
  std::unique_ptr<SemanticAnalyzer> semantic_analyzer(
      new SemanticAnalyzer(ast));

  bool is_valid = semantic_analyzer->validate();
  if (!is_valid) {
    std::cerr << "Failed semantic checking" << std::endl;
    exit(EXIT_FAILURE);
  }

  std::unique_ptr<CodeGenerator> code_gen(
      new CodeGenerator(ast, semantic_analyzer->sym_table, std::cout));

  std::cout << *ast << std::endl;
  code_gen->generate_wasm();
}

int main(int argc, char **argv) {
  yy::JayCompiler driver;

  if (argc >= 2) {
    std::string filename = argv[1];
    std::ifstream file{filename};
    if (!file.is_open()) {
      std::cerr << "File \"" << filename << "\" not found" << std::endl;
      return EXIT_SUCCESS;
    }

    build_ast(driver, &file, filename);
  } else {
    std::cerr << "Please specify the filename" << std::endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}