#define CATCH_CONFIG_MAIN
#include "Driver.hpp"
#include "catch.hpp"
#include <fstream>
#include <iostream>

TEST_CASE("single global variable node", "[ast]") {
  yy::Driver driver;
  std::ifstream simple("./test/parser/single_node.pass");
  auto *ast = driver.parse(&simple);

  SECTION("ast should be initialized") {
    REQUIRE_FALSE(ast == nullptr);
#ifdef SHOWAST
    if (ast) {
      std::cout << *ast << std::endl;
    }
#endif
  }

  SECTION("should contain only one direct child") {
    REQUIRE(ast->children.size() == 1);
  }

  SECTION("should contain only one global variable") {
    auto *global_var_node = ast->children[0];

    REQUIRE_FALSE(global_var_node == nullptr);

    auto children = global_var_node->children;
    REQUIRE(children.size() == 2);

    REQUIRE(children[0]->type == "int");
    REQUIRE(children[1]->type == "id");
  }
}
