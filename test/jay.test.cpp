#define CATCH_CONFIG_MAIN
#include "Driver.hpp"
#include "catch.hpp"
#include <fstream>
#include <iostream>

TEST_CASE("temp test", "[ast]") {
  std::ifstream simple("./test/parser/parse.t19");
  yy::Driver driver;
  auto *ast = driver.parse(&simple);
  std::cout << *ast << std::endl;
  REQUIRE_FALSE(false);
}