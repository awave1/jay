#pragma once
#include <iostream>
#include <string>

#include "parser.tab.hh"

#ifndef yyFlexLexerOnce
#include <FlexLexer.h>
#endif

class Lexer : public yyFlexLexer {
public: 
	Lexer(std::istream* is = nullptr, std::ostream* os = nullptr) 
		: yyFlexLexer(is, os)
	{}
	using TokenType = yy::Parser::semantic_type;
	int yylex();
	int lex(TokenType* semval) {
		m_val  = semval;
		return yylex();
	}
private:
	TokenType* m_val;
};
