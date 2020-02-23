/* Bison 
Parser ENBF -> LALR

parser.yy -> bison    -> parser.tab.cc
          -> bison -d -> parser.tab.hh

*/

/* Declarations */
%skeleton "lalr1.cc"
/* api.parser.class  in bison 3.5 */
%define "parser_class_name" {Parser}

%{
#include <iostream>
#include <string>

#include "AST.h"
%}

%error-verbose

%union {
	std::string* strVal;
	int intVal;
	struct ASTNode* nodeVal;
}

%token          MUL "*"
%token          LPAREN "("
%token          RPAREN ")"
%token <intVal> NUMBER "NUMBER"
%token          ADD "+"
%token          ERROR_TOK "ERROR"

%type <intVal> expr factor term expr2 term2

%start st

%parse-param { struct Driver& driver }


%{
#include "Lexer.h"
#undef  yylex
#define yylex driver.lexer->lex
%}



/* Grammars ENBF */
/* Rule format 
name-of-rule: tokens 
	| other case
	;
*/
%%

st: expr { driver.returnValue = $1; }
  ;

expr: term expr2   { $$ = $1 + $2; }
    ;

expr2: ADD term expr2 { $$ = $2 + $3; }
     |                 { $$ = 0; }
     ;



term: factor term2 { $$ = $1 * $2; }
    ;

term2: MUL factor term2 { $$ = $2 * $3; }
     |                  { $$ = 1; }
     ;

factor: NUMBER { $$ = $1; }
      | LPAREN expr RPAREN { $$ = $2; }
      ;

%%
/* C/C++ code */

// Error handling code!
void yy::Parser::error(std::string const& str) {
	std::cerr << "Error: " << str << "\n";
	exit(1);
}

