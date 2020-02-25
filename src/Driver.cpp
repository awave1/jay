#include "./include/Driver.h"

namespace yy {
ASTNode *Driver::parse(std::istream *is) {
  m_ast = nullptr;
  m_lexer = std::make_unique<Lexer>(is);
  m_parser = std::make_unique<Parser>(*this);
  m_parser->parse();
  return m_ast;
}

} // namespace yy

int main() {
  yy::Driver driver{};
  if (auto *ast = driver.parse(&std::cin)) {
    std::cout << "Success\n";
    std::cout << *ast << "\n";

  } else {
    std::cerr << "Failed parsing\n";
    return 1;
  }
  return 0;
}
