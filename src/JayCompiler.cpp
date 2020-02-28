#include "./include/JayCompiler.hpp"
#include <fstream>

namespace yy {

/**
 * @brief Parse the input from specified input stream and return resultig
 * Abstract Syntax Tree
 *
 * @param is input stream, can be file input or stdin
 * @param file filename that will be opened and parsed; used for better error
 * handling
 * @return ast_node_t* an Abstract Syntax Tree of the specified  input
 */
ast_node_t *JayCompiler::parse(std::istream *is, std::string file) {
  ast = nullptr;
  lexer = std::make_unique<Lexer>(is);
  parser = std::make_unique<Parser>(*this);
  filename = file;
#ifdef YYTRACE
  parser->set_debug_level(1);
#endif
  parser->parse();
  return ast;
}

} // namespace yy
