%option c++
%option yyclass="Lexer"

%{
#include "Lexer.h"
#include "parser.tab.hh"
%}

number [0-9]

%%

{number}+		{ val->intVal = atoi(yytext); 
			return yy::Parser::token::NUMBER; }


"*"			{ return yy::Parser::token::MUL; }
"+"			{ return yy::Parser::token::ADD; }
"("			{ return yy::Parser::token::LPAREN;}
")"			{ return yy::Parser::token::RPAREN; }

[ \t\r\n] 		/* do nothing */
.			std::cerr << "Lex error, unknown char " << yytext << "\n"; return yy::Parser::token::ERROR_TOK;

%%

int yyFlexLexer::yywrap() { return 1; }
