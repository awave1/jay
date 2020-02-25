%skeleton "lalr1.cc"
%define "parser_class_name" {Parser}

%debug

%{
  #include <iostream>
  #include <string>
  #include "src/include/ast.h"
%}

%parse-param { struct Driver& driver }
%error-verbose

%union {
  struct ASTNode *node;
  std::string *str;
  int number;
}

%token T_ID
%token T_STR
%token T_NUM
%token T_TYPE_INT
%token T_TYPE_BOOLEAN
%token T_TYPE_VOID
%token T_RESERVED_TRUE
%token T_RESERVED_FALSE
%token T_RESERVED_IF 
%token T_RESERVED_ELSE
%token T_RESERVED_WHILE
%token T_RESERVED_RETURN
%token T_RESERVED_BREAK
%token T_SEPARATOR_LPAREN
%token T_SEPARATOR_RPAREN
%token T_SEPARATOR_LBRACE
%token T_SEPARATOR_RBRACE
%token T_SEPARATOR_SEMI
%token T_SEPARATOR_COMMA
%token T_OP_PLUS
%token T_OP_MINUS
%token T_OP_TIMES
%token T_OP_DIV
%token T_OP_MOD
%token T_OP_GT
%token T_OP_LT
%token T_OP_GTEQ
%token T_OP_LTEQ
%token T_OP_EQ
%token T_OP_EQEQ
%token T_OP_NOT
%token T_OP_NOTEQ
%token T_OP_AND
%token T_OP_OR

%right '='
%left '+' '-'
%left '*' '/'
%nonassoc '!' T_OP_MINUS

%type<node> type
%type<node> literal
%type<node> identifier
%type<node> declarations
%type<node> declaration

%start program

%{
#include "./src/include/Driver.h"
#include "Lexer.h"

#undef yylex
#define yylex driver.m_lexer->lex
%}

%%

program: /* empty */
     | declarations { driver.m_ast = new ASTNode{ "program", {  } }; }
     ;

literal: T_NUM
       | T_STR
       | T_RESERVED_TRUE
       | T_RESERVED_FALSE
       ;

type: T_TYPE_INT
    | T_TYPE_BOOLEAN
    ;

identifier: T_ID
          ;

declarations: declaration
            | declarations declaration
            ;

declaration: variable_declaration
           | function_declaration
           | main_function_declaration
           ;

variable_declaration: type identifier T_SEPARATOR_SEMI
                    ;

function_declaration: function_header block
                    ;

function_header: type function_declarator
               | T_TYPE_VOID function_declarator
               ;

function_declarator: identifier T_SEPARATOR_LPAREN param_list T_SEPARATOR_RPAREN
                   | identifier T_SEPARATOR_LPAREN T_SEPARATOR_RPAREN
                   ;

main_function_declaration: main_function_declarator block
                         ;

main_function_declarator: identifier T_SEPARATOR_LPAREN T_SEPARATOR_RPAREN
                        ;

param_list: param
          | param_list T_SEPARATOR_COMMA param
          ;

param: type identifier
     ;

block: T_SEPARATOR_LBRACE block_statements T_SEPARATOR_RBRACE
     | T_SEPARATOR_LBRACE T_SEPARATOR_RBRACE
     ;

block_statements: block_statement
                | block_statements block_statement
                ;

block_statement: variable_declaration
               | statement
               ;

statement: block
         | T_SEPARATOR_SEMI
         | statement_expression T_SEPARATOR_SEMI
         | T_RESERVED_BREAK T_SEPARATOR_SEMI
         | T_RESERVED_RETURN T_SEPARATOR_SEMI
         | T_RESERVED_IF T_SEPARATOR_LPAREN expression T_SEPARATOR_RPAREN
         | T_RESERVED_IF T_SEPARATOR_LPAREN expression T_SEPARATOR_RPAREN T_RESERVED_ELSE
         | T_RESERVED_WHILE T_SEPARATOR_LPAREN expression T_SEPARATOR_RPAREN
         ;

statement_expression: assignment
                    | function_invocation
                    ;

function_invocation: identifier T_SEPARATOR_LPAREN args_list T_SEPARATOR_RPAREN
                   | identifier T_SEPARATOR_LPAREN T_SEPARATOR_RPAREN
                   ;

args_list: expression
         | args_list T_SEPARATOR_COMMA expression
         ;

primary: literal
       | T_SEPARATOR_LPAREN expression T_SEPARATOR_RPAREN
       ;

postfix_expression: identifier
                  | primary
                  ;

unary_expression: T_OP_MINUS unary_expression
                | T_OP_NOT unary_expression
                | postfix_expression
                ;

multiplicative_expression: unary_expression
                         | multiplicative_expression T_OP_TIMES unary_expression
                         | multiplicative_expression T_OP_DIV unary_expression
                         | multiplicative_expression T_OP_MOD unary_expression
                         ;

additive_expression: multiplicative_expression
                   | additive_expression T_OP_PLUS multiplicative_expression
                   | additive_expression T_OP_MINUS multiplicative_expression
                   ;

relational_expression: additive_expression
                     | relational_expression T_OP_LT additive_expression
                     | relational_expression T_OP_GT additive_expression
                     | relational_expression T_OP_LTEQ additive_expression
                     | relational_expression T_OP_GTEQ additive_expression
                     ;

equality_expression: relational_expression
                   | equality_expression T_OP_EQEQ relational_expression
                   | equality_expression T_OP_NOTEQ relational_expression
                   ;

conditional_and_expression: equality_expression
                          | conditional_and_expression T_OP_AND equality_expression
                          ;

conditional_or_expression: conditional_and_expression
                         | conditional_or_expression T_OP_OR conditional_and_expression
                         ;

assignment_expression: conditional_or_expression
                     | assignment
                     ;

assignment: identifier T_OP_EQ assignment_expression
          ;

expression: assignment_expression
          ;

%%

void yy::Parser::error(std::string const& msg) {
	std::cerr << "Error: " << msg << "\n";
	exit(1);
}
