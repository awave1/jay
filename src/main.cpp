#include "./include/JayCompiler.hpp"
#include <fstream>
#include <iostream>

void build_ast(yy::JayCompiler &driver, std::istream *is, std::string file) {
  auto *ast = driver.parse(is, file);

  if (ast) {
    std::cout << *ast << std::endl;
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
    build_ast(driver, &std::cin, "");
  }

  return 0;
}