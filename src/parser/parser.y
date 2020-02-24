%{
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "../include/ast.h"

extern FILE *yyin;

extern void yyerror(const char* err);
extern int yydebug;
extern char* yytext;

%}

%union {
  char *str;
  ast_node_t* tree;
}

%token<str> T_ID
%token T_STR
%token T_NUM
%token T_TYPE_INT T_TYPE_BOOLEAN T_TYPE_VOID
%token T_RESERVED_TRUE T_RESERVED_FALSE T_RESERVED_IF T_RESERVED_ELSE T_RESERVED_WHILE T_RESERVED_RETURN T_RESERVED_BREAK
%token T_SEPARATOR_LPAREN T_SEPARATOR_RPAREN T_SEPARATOR_LBRACE T_SEPARATOR_RBRACE T_SEPARATOR_SEMI T_SEPARATOR_COMMA
%token T_OP_PLUS T_OP_MINUS T_OP_TIMES T_OP_DIV T_OP_MOD T_OP_GT T_OP_LT T_OP_GTEQ T_OP_LTEQ T_OP_EQ T_OP_EQEQ T_OP_NOT T_OP_NOTEQ T_OP_AND T_OP_OR

%type<str> identifier
%type<tree> start

%start start

%%

start: /* empty */
     | declarations { $$ = make_new_node("start"); }
     ;

literal: T_NUM
       | T_STR
       | T_RESERVED_TRUE
       | T_RESERVED_FALSE
       ;

type:
    | T_TYPE_INT
    | T_TYPE_BOOLEAN
    ;

identifier: T_ID {
  $$ = strdup(yytext);
}
          ;

declarations: declaration
            | declarations declaration
            ;

declaration: variable_declaration
           | function_declaration
           | main_function_declaration
           ;

variable_declaration: /* empty */
                    | type identifier T_SEPARATOR_SEMI
                    ;

function_declaration: function_header block
                    ;

function_header: type function_declarator
               | T_TYPE_VOID function_declarator
               ;

function_declarator: identifier T_SEPARATOR_LPAREN param_list T_SEPARATOR_RPAREN
                   | identifier T_SEPARATOR_LPAREN T_SEPARATOR_RPAREN
                   ;

main_function_declaration: main_function_declarator block {
  printf("> main function: declaration\n");
}
                         ;

main_function_declarator: identifier T_SEPARATOR_LPAREN T_SEPARATOR_RPAREN {
  printf("> main function: identifier = %s, declarator\n", $1);
}
                        ;

param_list: param
          | param T_SEPARATOR_COMMA param_list
          ;

param: type identifier
     ;

block: T_SEPARATOR_LBRACE block_statements T_SEPARATOR_RBRACE { printf("> block: statements\n"); }
     | T_SEPARATOR_LBRACE T_SEPARATOR_RBRACE { printf("> block: empty"); }
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

expression: assignment_expression { 

}
          ;

%%

int main(int argc, char **argv) {
  if (argc == 3) {
    yydebug = atoi(argv[1]);
    yyin = fopen(argv[2], "r");
  }

  if (argc == 2) {
    if (isdigit(argv[1][0])) {
      yydebug = atoi(argv[1]);
    } else {
      yyin = fopen(argv[1], "r");
    }
  }

  yyparse();
  return 0;
}

void yyerror(const char* err) {
  fprintf(stderr, "error: %s\n", err);
}
