#define CATCH_CONFIG_MAIN
#include "Driver.hpp"
#include "catch.hpp"
#include <fstream>
#include <iostream>

std::string file(std::string test_name) { return "./test/parser/" + test_name; }

TEST_CASE("single_node.pass: single global variable node", "[ast]") {
  yy::Driver driver;
  std::ifstream testfile(file("single_node.pass"));
  auto *ast = driver.parse(&testfile);

  SECTION("AST should be initialized") {
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

    REQUIRE(children[0]->type == ast_node_t::Node::int_t);
    REQUIRE(children[1]->type == ast_node_t::Node::id);
  }
}

TEST_CASE("parse.t2.fail : unterminated variable declaration", "[ast]") {
  yy::Driver driver;
  std::ifstream testfile(file("parse.t2.failj"));
  auto *ast = driver.parse(&testfile);

  SECTION("AST should not be initialized") { REQUIRE(ast == nullptr); }
}

TEST_CASE("parse.t19.pass", "[ast]") {
  yy::Driver driver;
  std::ifstream testfile(file("parse.t19.pass"));
  auto *ast = driver.parse(&testfile);

  SECTION("ast should be initialized") {
    REQUIRE_FALSE(ast == nullptr);
#ifdef SHOWAST
    if (ast) {
      std::cout << *ast << std::endl;
    }
#endif
  }

  SECTION("should contain a main function declaration node") {
    auto children = ast->children;
    REQUIRE_FALSE(children.empty());

    auto *main_func_node = children[0];
    REQUIRE(main_func_node->type == ast_node_t::Node::main_func_decl);
  }
}

TEST_CASE("parse.t21.pass", "[ast]") {
  yy::Driver driver;
  std::ifstream testfile(file("parse.t21.pass"));
  auto *ast = driver.parse(&testfile);

  SECTION("AST should be initialized") {
    REQUIRE_FALSE(ast == nullptr);
#ifdef SHOWAST
    if (ast) {
      std::cout << *ast << std::endl;
    }
#endif
  }

  SECTION("") {}
}

TEST_CASE("parse.t22.pass", "[ast]") {
  yy::Driver driver;
  std::ifstream testfile(file("parse.t22.pass"));
  auto *ast = driver.parse(&testfile);

  SECTION("ast should be initialized") {
    REQUIRE_FALSE(ast == nullptr);
#ifdef SHOWAST
    if (ast) {
      std::cout << *ast << std::endl;
    }
#endif
  }
}

TEST_CASE("gen.t18.pass", "[ast]") {
  yy::Driver driver;
  std::ifstream testfile(file("gen.t18.pass"));
  auto *ast = driver.parse(&testfile);

  SECTION("ast should be initialized") {
    REQUIRE_FALSE(ast == nullptr);
#ifdef SHOWAST
    if (ast) {
      std::cout << *ast << std::endl;
    }
#endif
  }
}