#ifndef JAY_COMPILER_H
#define JAY_COMPILER_H

#include <memory>

#include "ASTNode.hpp"
#include "Lexer.hpp"
#include "SemanticAnalyzer.hpp"
#include "parser.tab.hpp"

namespace yy {

/**
 * @brief JayCompiler binds together flex lexer and bison parser
 */
struct JayCompiler {
  JayCompiler() = default;
  JayCompiler(JayCompiler const &) = delete;

  std::unique_ptr<Lexer> lexer;
  std::unique_ptr<Parser> parser;
  std::unique_ptr<SemanticAnalyzer> semanticAnalyzer;
  ASTNode *ast;
  std::string filename;

  /**
   * @brief Parse the input from specified input stream and return resultig
   * Abstract Syntax Tree
   *
   * @param is input stream, can be file input or stdin
   * @param file filename that will be opened and parsed; used for better error
   * handling
   * @return ASTNode* an Abstract Syntax Tree of the specified  input
   */
  ASTNode *parse(std::istream *is, std::string file);
};

} // namespace yy

#endif /* JAY_COMPILER_H */