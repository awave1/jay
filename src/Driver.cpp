#include "./include/Driver.hpp"
#include <fstream>

namespace yy {

ast_node_t *Driver::parse(std::istream *is, std::string file) {
  ast = nullptr;
  m_lexer = std::make_unique<Lexer>(is);
  m_parser = std::make_unique<Parser>(*this);
  filename = file;
#ifdef YYTRACE
  m_parser->set_debug_level(1);
#endif
  m_parser->parse();
  return ast;
}

} // namespace yy
