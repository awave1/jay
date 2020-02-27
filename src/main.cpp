#include "./include/Driver.hpp"
#include <fstream>
#include <iostream>

void build_ast(yy::Driver &driver, std::istream *is) {
  auto *ast = driver.parse(is);

  if (ast) {
    std::cout << *ast << std::endl;
  } else {
    std::cerr << "Failed parsing" << std::endl;
    exit(1);
  }
}

int main(int argc, char **argv) {
  yy::Driver driver;

  if (argc >= 2) {
    std::ifstream file{argv[1]};
    if (!file.is_open()) {
      return 1;
    } else {
      build_ast(driver, &file);
    }
  } else {
    build_ast(driver, &std::cin);
  }

  return 0;
}