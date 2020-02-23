%option c++
%option yyclass="Lexer"

%{
#include "Lexer.h"
#include "parser.tab.hh"
%}

%%

[a-zA-Z][a-zA-Z0-9]* 	{ m_val->strVal = new std::string{yytext}; return yy::Parser::token::id; }
"="			return yy::Parser::token::eq;
"*"			return yy::Parser::token::asterisk;

[ \t\r\n] 		/* do nothing */
.			std::cerr << "Lex error, unknown char " << yytext << "\n"; return yy::Parser::token::error_tok;

%%

int yyFlexLexer::yywrap() { return 1; }
