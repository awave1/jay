#include "Lexer.h"
#include <iostream>

int yy::Driver::parse(std::istream *is) {
  ast = nullptr;
  lexer = new Lexer(is);
  parser = new yy::Parser(*this);
  return parser->parse();
}

int main() {
  yy::Driver dr;
  int return_status = dr.parse(&std::cin);

  if (return_status) {
    std::cout << "failed parse!\n";
  } else {
    std::cout << "Value: " << dr.returnValue << "\n";
  }

  return ret;
}
