#define CATCH_CONFIG_MAIN

#include "../lib/catch2/catch.hpp"
#include "../src/include/JayCompiler.hpp"
#include <fstream>
#include <iostream>
#include <memory>

std::string file(std::string test_name) {
  return "./test/semantic/" + test_name;
}

TEST_CASE("simple.pass: passes semantic cheching") {
  yy::JayCompiler driver;
  std::ifstream testfile(file("simple.pass"));

  std::shared_ptr<yy::ast_node_t> ast(
      driver.parse(&testfile, file("simple.pass")));
  driver.semanticAnalyzer = std::make_unique<SemanticAnalyzer>(ast);

  std::cout << *driver.semanticAnalyzer->get_ast() << std::endl;

  REQUIRE(driver.semanticAnalyzer->validate());
}