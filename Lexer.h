#ifndef LEXER_H
#define LEXER_H

#include <iostream>
#include <string>

#include "parser.tab.hh"

#ifndef yyFlexLexerOnce
#include <FlexLexer.h>
#endif

/**
 * @brief Lexer is C++ wrapper for flex, it inherits yyFlexLexer that is
 * provided by the `flex`
 */
class Lexer : public yyFlexLexer {
public:
  using TokenType = yy::Parser::semantic_type;

  Lexer(std::istream *is = nullptr, std::ostream *os = nullptr)
      : yyFlexLexer(is, os) {}

  int yylex();

  int lex(TokenType *semval) {
    m_val = semval;
    return yylex();
  }

private:
  TokenType *m_val;
};

#endif /* LEXER_H */