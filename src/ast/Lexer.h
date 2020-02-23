#pragma once
#include <string>
#include <iostream>

#ifndef yyFlexLexerOnce 
#include <FlexLexer.h>
#endif

#include "parser.tab.hh"
#include "AST.h"

struct Lexer : public yyFlexLexer {
	Lexer(std::istream* is) 
		: yyFlexLexer(is, &std::cout)
	{}

	using TokenType = yy::Parser::semantic_type;

	int yylex();
	int lex(TokenType* v) {  
		val = v; 
		return yylex(); 
	}

	TokenType* val;
};

namespace yy {
struct Driver {
	int parse(std::istream* is);
	Lexer* lexer;
	yy::Parser* parser;
	ASTNode* ast;
	int returnValue;
};

}
