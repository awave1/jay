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
  struct ast_node_t *node;
  std::string *str;
  std::vector<struct ast_node_t *> *list;
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

%type<str> literal

%type<node> type
%type<node> identifier
%type<list> globaldeclarations
%type<node> globaldeclaration
%type<list> variable_declaration
%type<list> function_declaration 
%type<node> main_function_declaration

%type<node> block
%type<list> block_statements
%type<node> block_statement
%type<node> statement

%type<node> assignment
%type<node> assignment_expression
%type<node> statement_expression

%type<node> param_list
%type<node> param

%start program

%{
  #include "./src/include/Driver.h"
  #include "Lexer.h"

  #undef yylex
  #define yylex driver.m_lexer->lex
%}

%%

program: /* empty */
     | globaldeclarations {
         std::vector<ast_node_t *> nodes = *$1;
         driver.ast = new ast_node_t{ "program", "program", "", nodes };
       }
     ;

literal: T_NUM {
           *$$ = std::string(driver.m_lexer->YYText());
         }
       | T_STR {
           *$$ = std::string(driver.m_lexer->YYText());
         }
       | T_RESERVED_TRUE {
           *$$ = "true";
         }
       | T_RESERVED_FALSE {
           *$$ = "false";
         }
       ;

type: T_TYPE_INT {
        $$ = new ast_node_t{ "int", "int", "", {} };
      }
    | T_TYPE_BOOLEAN {
        $$ = new ast_node_t{ "boolean", "boolean", "", {} };
      }
    ;

identifier: T_ID {
              $$ = new ast_node_t{ "id", "id", std::string(driver.m_lexer->YYText()), {} };
            }
          ;

globaldeclarations: globaldeclaration {
                      $$ = new std::vector<ast_node_t *>();
                      $$->push_back($1);
                    }
                  | globaldeclarations globaldeclaration {
                      $$ = $1;
                      $$->push_back($2);
                    }
                  ;

globaldeclaration: variable_declaration {
                     std::vector<ast_node_t *> global_var_nodes = *$1;
                     $$ = new ast_node_t { "globalVarDecl", "globalVarDecl", "", global_var_nodes };
                   }
                 | function_declaration {
                   std::vector<ast_node_t *> func_decl_nodes = *$1;
                   $$ = new ast_node_t { "funcDecl", "funcDecl", "", func_decl_nodes };
                 }
                 | main_function_declaration {
                     ast_node_t *main_func_node = $1;
                     $$ = main_func_node; 
                   } 
                 ;

variable_declaration: type identifier T_SEPARATOR_SEMI {
                        auto *nodes = new std::vector<ast_node_t *>();

                        nodes->push_back($1);
                        nodes->push_back($2);
                        $$ = nodes;
                      }
                    ;

function_declaration: type identifier T_SEPARATOR_LPAREN T_SEPARATOR_RPAREN block {
                        auto *func_nodes = new std::vector<ast_node_t *>();
                        auto *empty_formals = new ast_node_t { "formals", "formals", "", {} };

                        func_nodes->push_back($1);
                        func_nodes->push_back($2);
                        func_nodes->push_back(empty_formals);
                        func_nodes->push_back($5);
                        $$ = func_nodes;
                      }
                    | type identifier T_SEPARATOR_LPAREN param_list T_SEPARATOR_RPAREN block {
                        std::vector<ast_node_t *> *func_nodes = new std::vector<ast_node_t *>();

                        func_nodes->push_back($1);
                        func_nodes->push_back($2);
                        func_nodes->push_back($4);
                        func_nodes->push_back($6);
                        $$ = func_nodes;
                      }
                    | T_TYPE_VOID identifier T_SEPARATOR_LPAREN T_SEPARATOR_RPAREN block {
                        ast_node_t *void_t_node = new ast_node_t { "void", "void", "", {} };
                        std::vector<ast_node_t *> *func_nodes = new std::vector<ast_node_t *>();

                        func_nodes->push_back(void_t_node);
                        func_nodes->push_back($2);
                        func_nodes->push_back($5);
                        $$ = func_nodes;
                      }
                    | T_TYPE_VOID identifier T_SEPARATOR_LPAREN param_list T_SEPARATOR_RPAREN block {
                        auto *void_t_node = new ast_node_t { "void", "void", "", {} };
                        auto *func_nodes = new std::vector<ast_node_t *>();

                        func_nodes->push_back(void_t_node);
                        func_nodes->push_back($2);
                        func_nodes->push_back($4);
                        func_nodes->push_back($6);
                        $$ = func_nodes;
                      }
                    ;

main_function_declaration: identifier T_SEPARATOR_LPAREN T_SEPARATOR_RPAREN block {
                             auto *void_t_node = new ast_node_t { "void", "void", "", {} };
                             auto *empty_formals = new ast_node_t { "formals", "formals", "", {} }; 

                             $$ = new ast_node_t {
                               "mainDecl",
                               "mainDecl",
                               "",
                               { void_t_node, $1, empty_formals, $4 }
                             };
                           }
                         ;
param_list: param {
              $$ = new ast_node_t{ "formals", "formals", "", { $1 } };
            }
          | param_list T_SEPARATOR_COMMA param {
              $$ = $1;
              $$->children.push_back($3);
              std::cout << "FORMALS (children) " <<  $$->children.size() << "\n";
            }
          ;

param: type identifier {
         ast_node_t *formal_param_node = new ast_node_t { "formal", "formal", "", { $1, $2 } };
         $$ = formal_param_node;
       }
     ;

block: T_SEPARATOR_LBRACE T_SEPARATOR_RBRACE {
       $$ = new ast_node_t {
         "block",
         "block",
         "",
         {}
       };
     }
     | T_SEPARATOR_LBRACE block_statements T_SEPARATOR_RBRACE {
       auto *block_children = $2;
       auto *block = new ast_node_t { "block", "block", "", *block_children };

       $$ = block;
     }
     ;

block_statements: block_statement {
                    auto *block_statements = new std::vector<ast_node_t *>();
                    block_statements->push_back($1);
                    $$ = block_statements;
                  }
                | block_statements block_statement {
                    $$ = $1;
                    $$->push_back($2); 
                  }
                ;

block_statement: variable_declaration {
                   auto *var_decl_node = new ast_node_t{ "varDecl", "varDecl", "", *$1 };
                   $$ = var_decl_node;
                 }
               | statement {
                   $$ = $1;
                 }
               ;

statement: block                                   { $$ = $1; }
         | T_SEPARATOR_SEMI
         | statement_expression T_SEPARATOR_SEMI   { $$ = $1; }
         | T_RESERVED_BREAK T_SEPARATOR_SEMI
         | T_RESERVED_RETURN T_SEPARATOR_SEMI
         | T_RESERVED_IF T_SEPARATOR_LPAREN expression T_SEPARATOR_RPAREN
         | T_RESERVED_IF T_SEPARATOR_LPAREN expression T_SEPARATOR_RPAREN T_RESERVED_ELSE
         | T_RESERVED_WHILE T_SEPARATOR_LPAREN expression T_SEPARATOR_RPAREN
         ;

statement_expression: assignment { $$ = new ast_node_t{ "statementExpr", "statementExpr", "", { $1 } }; }
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

assignment: identifier T_OP_EQ assignment_expression { $$ = new ast_node_t{ "=", "=", "", { $1, $3 } }; }
          ;

expression: assignment_expression
          ;

%%

void yy::Parser::error(std::string const& msg) {
	std::cerr << "Error: " << msg << "\n";
	exit(1);
}
