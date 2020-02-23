%skeleton "lalr1.cc"
%define api.parser.class {Parser}
%{
#include <iostream>
#include "AST.h"
%}


%parse-param { struct Driver& driver }
%error-verbose

%union {
	std::string* strVal;
	struct ASTNode* nodeVal;
}

%token <strVal>	id 		"id"
%token 		eq 		"eq"
%token 		asterisk	"asterisk"
%token 		error_tok	"error token"

%start START

%type <nodeVal> S L R

%destructor { delete $$; } id

%{
#include "Driver.h"
#include "Lexer.h"

#undef yylex
#define yylex driver.m_lexer->lex

%}

%%

START: S { driver.m_ast = $1; }
     ;
S: L eq R { $$ = new ASTNode{"=", {$1, $3}}; }
 | R      { $$ = $1; }
 ;

L: asterisk R { $$ = new ASTNode{"*", {$2}}; }
 | id         { $$ = new ASTNode{*$1, {}}; }
 ;

R: L          { $$ = $1; }
 ;

%%

void yy::Parser::error(std::string const& msg) {
	std::cerr << "Error: " << msg << "\n";
	exit(1);
}
