#include "./include/Driver.h"
#include <fstream>

namespace yy {

ASTNode *Driver::parse(std::istream *is) {
  m_ast = nullptr;
  m_lexer = std::make_unique<Lexer>(is);
  m_parser = std::make_unique<Parser>(*this);
  m_parser->parse();
  return m_ast;
}

} // namespace yy

int main(int argc, char **argv) {
  yy::Driver driver{};
  std::istream *is;

  if (argc >= 2) {
    std::ifstream file{argv[1]};
    if (!file.is_open()) {
      return 1;
    } else {
      is = &file;
    }
  } else {
    is = &std::cin;
  }

  auto *ast = driver.parse(is);

  if (ast) {
    std::cout << "Success" << std::endl;
    std::cout << *ast << std::endl;
  } else {
    std::cerr << "Failed parsing" << std::endl;
    return 1;
  }

  return 0;
}
