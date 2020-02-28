#define CATCH_CONFIG_MAIN
#include "JayCompiler.hpp"
#include "catch.hpp"
#include <cstring>
#include <fstream>
#include <iostream>

std::string file(std::string test_name) { return "./test/parser/" + test_name; }

TEST_CASE("empty.fail: empty file", "[ast]") {
  yy::JayCompiler driver;
  std::ifstream testfile(file("empty.fail"));
  auto *ast = driver.parse(&testfile, file("empty.fail"));

  SECTION("AST should not be initialized") { REQUIRE(ast == nullptr); }
}

TEST_CASE("single_node.pass: single global variable node", "[ast]") {
  yy::JayCompiler driver;
  std::ifstream testfile(file("single_node.pass"));
  auto *ast = driver.parse(&testfile, file("single_node.pass"));

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

TEST_CASE(
    "global_var_assign.fail: trying to set global variable in global scope",
    "[ast]") {
  yy::JayCompiler driver;
  std::ifstream testfile(file("global_var_assign.fail"));
  auto *ast = driver.parse(&testfile, file("global_var_assign.fail"));

  SECTION("AST should not be initialized") { REQUIRE(ast == nullptr); }
}

TEST_CASE("nested_function.fail: trying to create a nested function", "[ast]") {
  yy::JayCompiler driver;
  std::ifstream testfile(file("nested_function.fail"));
  auto *ast = driver.parse(&testfile, file("nested_function.fail"));

  SECTION("AST should not be initialized") { REQUIRE(ast == nullptr); }
}

TEST_CASE("return_func_invocation.pass: return a function call that calls "
          "another function",
          "[ast]") {
  yy::JayCompiler driver;
  std::ifstream testfile(file("return_func_invocation.pass"));
  auto *ast = driver.parse(&testfile, file("return_func_invocation.pass"));

  SECTION("AST should be initialized") {
    REQUIRE_FALSE(ast == nullptr);
#ifdef SHOWAST
    if (ast) {
      std::cout << *ast << std::endl;
    }
#endif
  }

  SECTION("should contain a function foo") {
    auto *function_decl = ast->find_first(ast_node_t::Node::function_decl);
    REQUIRE_FALSE(function_decl == nullptr);

    auto *id_node = function_decl->find_first(ast_node_t::Node::id);
    REQUIRE_FALSE(id_node == nullptr);
    REQUIRE_THAT(id_node->value, Catch::Matchers::Equals("foo"));

    auto func_calls =
        function_decl->find_recursive(ast_node_t::Node::function_call);
    REQUIRE(func_calls.size() == 2);
  }
}

TEST_CASE("null_str.pass: string with null character in the middle", "[ast]") {
  yy::JayCompiler driver;
  std::ifstream testfile(file("null_str.pass"));
  auto *ast = driver.parse(&testfile, file("null_str.pass"));

  SECTION("AST should be initialized") {
    REQUIRE_FALSE(ast == nullptr);
#ifdef SHOWAST
    if (ast) {
      std::cout << *ast << std::endl;
    }
#endif
  }

  SECTION("should contain a string 'abcd'") {
    auto string_node = ast->find_recursive(ast_node_t::Node::string)[0];
    REQUIRE_FALSE(string_node == nullptr);

    auto value = string_node->value;
    REQUIRE_THAT(value, Catch::Matchers::Equals("abcd"));
  }
}

TEST_CASE("comment_in_expression.fail: if statement contains a comment",
          "[ast]") {
  yy::JayCompiler driver;
  std::ifstream testfile(file("comment_in_expression.fail"));
  auto *ast = driver.parse(&testfile, file("comment_in_expression.fail"));

  SECTION("AST should not be initialized") { REQUIRE(ast == nullptr); }
}

TEST_CASE("c_style_while.pass: c style while loop", "[ast]") {
  yy::JayCompiler driver;
  std::ifstream testfile(file("c_style_while.pass"));
  auto *ast = driver.parse(&testfile, file("c_style_while.pass"));

  SECTION("AST should be initialized") {
    REQUIRE_FALSE(ast == nullptr);
#ifdef SHOWAST
    if (ast) {
      std::cout << *ast << std::endl;
    }
#endif
  }

  SECTION("should contain expression in while") {
    auto *while_node =
        ast->find_recursive(ast_node_t::Node::while_statement)[0];
    REQUIRE_FALSE(while_node == nullptr);

    auto *not_eq_expr = while_node->find_first(ast_node_t::Node::noteq_op);
    REQUIRE_FALSE(not_eq_expr == nullptr);
    REQUIRE(not_eq_expr->children.size() == 2);
  }
}

TEST_CASE("parse.t2.fail : unterminated variable declaration", "[ast]") {
  yy::JayCompiler driver;
  std::ifstream testfile(file("parse.t2.fail"));
  auto *ast = driver.parse(&testfile, file("parse.t2.fail"));

  SECTION("AST should not be initialized") { REQUIRE(ast == nullptr); }
}

TEST_CASE("parse.t19.pass: main function with nested if-else statement and "
          "null block statements",
          "[ast]") {
  yy::JayCompiler driver;
  std::ifstream testfile(file("parse.t19.pass"));
  auto *ast = driver.parse(&testfile, file("parse.t19.pass"));

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

  SECTION("should contain 2 null block statements") {
    auto main_node = ast->children[0];

    auto null_statements =
        ast->find_recursive(ast_node_t::Node::null_statement);
    REQUIRE_FALSE(null_statements.empty());
    REQUIRE(null_statements.size() == 2);
  }
}

TEST_CASE("parse.t21.pass: main function with math expressions", "[ast]") {
  yy::JayCompiler driver;
  std::ifstream testfile(file("parse.t21.pass"));
  auto *ast = driver.parse(&testfile, file("parse.t21.pass"));

  SECTION("AST should be initialized") {
    REQUIRE_FALSE(ast == nullptr);
#ifdef SHOWAST
    if (ast) {
      std::cout << *ast << std::endl;
    }
#endif
  }

  SECTION("should contain 2 statement expressions") {
    auto statement_exprs =
        ast->find_recursive(ast_node_t::Node::statement_expr);

    REQUIRE(statement_exprs.size() == 2);

    SECTION("first expression should be multiply then add") {
      auto *first_expr = statement_exprs[0];
      auto *add_op = first_expr->find_recursive(ast_node_t::Node::add_op)[0];
      REQUIRE_FALSE(add_op == nullptr);
      REQUIRE(add_op->type == ast_node_t::Node::add_op);
      auto *add_mul_child =
          first_expr->find_recursive(ast_node_t::Node::mul_op)[0];
      REQUIRE_FALSE(add_mul_child == nullptr);
      REQUIRE(add_mul_child->type == ast_node_t::Node::mul_op);
      REQUIRE(add_mul_child->children.size() == 2);
    }

    SECTION("second expression should be add then multiply") {
      auto *second_expr = statement_exprs[1];
      auto *mul_op = second_expr->find_recursive(ast_node_t::Node::mul_op)[0];
      REQUIRE_FALSE(mul_op == nullptr);
      REQUIRE(mul_op->type == ast_node_t::Node::mul_op);
      auto *mul_add_child =
          second_expr->find_recursive(ast_node_t::Node::add_op)[0];
      REQUIRE_FALSE(mul_add_child == nullptr);
      REQUIRE(mul_add_child->type == ast_node_t::Node::add_op);
      REQUIRE(mul_add_child->children.size() == 2);
    }
  }
}

TEST_CASE("parse.t22.pass: main function with unary minus sign expressions",
          "[ast]") {
  yy::JayCompiler driver;
  std::ifstream testfile(file("parse.t22.pass"));
  auto *ast = driver.parse(&testfile, file("parse.t22.pass"));

  SECTION("ast should be initialized") {
    REQUIRE_FALSE(ast == nullptr);
#ifdef SHOWAST
    if (ast) {
      std::cout << *ast << std::endl;
    }
#endif
  }

  SECTION("should contain 3 statement expressions") {
    auto statement_exprs =
        ast->find_recursive(ast_node_t::Node::statement_expr);
    REQUIRE(statement_exprs.size() == 3);
  }
}

TEST_CASE("gen.t18.pass: recursive descend parser implemented in j--",
          "[ast]") {
  yy::JayCompiler driver;
  std::ifstream testfile(file("gen.t18.pass"));
  auto *ast = driver.parse(&testfile, file("gen.t18.pass"));

  SECTION("ast should be initialized") {
    REQUIRE_FALSE(ast == nullptr);
#ifdef SHOWAST
    if (ast) {
      std::cout << *ast << std::endl;
    }
#endif
  }

  SECTION("should contain 13 (12 functions + 1 main) function declarations") {
    auto *main_func = ast->find_first(ast_node_t::Node::main_func_decl);
    REQUIRE_FALSE(main_func == nullptr);
    REQUIRE(main_func->type == ast_node_t::Node::main_func_decl);

    auto func_exprs = ast->find_all(ast_node_t::Node::function_decl);
    REQUIRE(func_exprs.size() == 12);
  }
}