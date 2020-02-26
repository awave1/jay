%skeleton "lalr1.cc"
%define "parser_class_name" {Parser}

%debug

%{
#include <iostream>
#include <string>
#include "src/include/ast.h"
#define YYDEBUG 1
%}

%parse-param { struct Driver& driver }
%error-verbose

%union {
  struct ast_node_t *node;
  std::vector<struct ast_node_t *> *list;
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

%type<node> literal

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

%type<node> param_list
%type<node> param

%type<list> actuals

%type<node> expression
%type<node> assignment
%type<node> assignment_expression
%type<node> statement_expression
%type<node> function_invocation
%type<node> unary_expression
%type<node> postfix_expression
%type<node> conditional_or_expression
%type<node> conditional_and_expression
%type<node> equality_expression
%type<node> relational_expression
%type<node> additive_expression
%type<node> multiplicative_expression


%type<node> primary

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
           auto num_val = std::string(driver.m_lexer->YYText());
           auto *num_node = new ast_node_t { "number", "number", num_val, {} };
           $$ = num_node;
         }
       | T_STR {
           auto str_val = std::string(driver.m_lexer->YYText());
           auto *str_node = new ast_node_t { "string", "string", str_val, {} };
           $$ = str_node;
         }
       | T_RESERVED_TRUE {
           auto *true_node = new ast_node_t { "boolean", "true", "true", {} };
           $$ = true_node;
         }
       | T_RESERVED_FALSE {
           auto *false_node = new ast_node_t { "boolean", "false", "false", {} };
           $$ = false_node;
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

                             $$ = new ast_node_t {"mainDecl",
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
       $$ = new ast_node_t { "block", "block", "", {} };
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

statement: block {
             $$ = $1;
           }
         | T_SEPARATOR_SEMI {
             auto *null_statement = new ast_node_t { "nullStmt", "nullStmt", "", {} };
             $$ = null_statement;
           }
         | statement_expression T_SEPARATOR_SEMI {
             $$ = $1;
           }
         | T_RESERVED_BREAK T_SEPARATOR_SEMI {
             auto *break_statement = new ast_node_t { "break", "break", "", {} };
             $$ = break_statement;
           }
         | T_RESERVED_RETURN expression T_SEPARATOR_SEMI {
             auto *return_statement = new ast_node_t { "return", "return", "", { $2 } };
             $$ = return_statement;
           }
         | T_RESERVED_RETURN T_SEPARATOR_SEMI {
             auto *return_statement = new ast_node_t { "return", "return", "", {} };
             $$ = return_statement;
           }
         | T_RESERVED_IF T_SEPARATOR_LPAREN expression T_SEPARATOR_RPAREN statement {
             auto *expression_node = $3;
             auto *statement_node = $5;
             auto *if_node = new ast_node_t { "if", "if", "", { expression_node, statement_node } };
             $$ = if_node;
           }
         | T_RESERVED_IF T_SEPARATOR_LPAREN expression T_SEPARATOR_RPAREN statement T_RESERVED_ELSE statement {
             auto *expression_node = $3;
             auto *if_statement_node = $5;
             auto *else_statement_node = $7;
             auto *if_else_node = new ast_node_t { "ifElse", "ifElse", "", { expression_node, if_statement_node, else_statement_node } };
             $$ = if_else_node;
           }
         | T_RESERVED_WHILE T_SEPARATOR_LPAREN expression T_SEPARATOR_RPAREN statement {
             auto *expression_node = $3;
             auto *while_statement_node = $5;
             auto *while_node = new ast_node_t { "while", "while", "", { expression_node, while_statement_node } };
             $$ = while_node;
           }
         ;

statement_expression: assignment {
                        $$ = new ast_node_t{ "statementExpr", "statementExpr", "", { $1 } };
                      }
                    | function_invocation {
                        $$ = new ast_node_t { "statementExpr", "statementExpt", "", { $1 } };
                      }
                    ;

function_invocation: identifier T_SEPARATOR_LPAREN actuals T_SEPARATOR_RPAREN {
                       auto *actuals_node = new ast_node_t { "actuals", "actuals", "", *$3 };
                       auto *func_call_node = new ast_node_t { "funcCall", "funcCall", "", { $1, actuals_node } };
                       $$ = func_call_node;
                     }
                   | identifier T_SEPARATOR_LPAREN T_SEPARATOR_RPAREN {
                       auto *actuals_node = new ast_node_t { "actuals", "actuals", "", {} };
                       auto *func_call_node = new ast_node_t { "funcCall", "funcCall", "", { $1, actuals_node } };
                       $$ = func_call_node;
                     }
                   ;

actuals: expression {
           auto *actuals_list = new std::vector<ast_node_t *>();
           actuals_list->push_back($1);
           $$ = actuals_list;
         }
       | actuals T_SEPARATOR_COMMA expression {
           $$ = $1;
           $$->push_back($3);
         }
       ;

primary: literal {
           $$ = $1;
         }
       | T_SEPARATOR_LPAREN expression T_SEPARATOR_RPAREN {
           $$ = $2;
         }
       | function_invocation {
           $$ = $1;
         }
       ;

postfix_expression: identifier {
                      $$ = $1;
                    }
                  | primary {
                      $$ = $1;
                    }
                  ;

unary_expression: T_OP_MINUS unary_expression {
                    auto *node = $2;
                    if (node && node->is_num()) {
                      node->attr = node->attr.insert(0, 1, '-');
                      $$ = node;
                    } else {
                      auto *unary_minus_node = new ast_node_t{ "-", "-", "", { node } };
                      $$ = unary_minus_node;
                    }
                  }
                | T_OP_NOT unary_expression {
                    auto *not_node = new ast_node_t{ "!", "!", "", { $2 } };
                    $$ = not_node;
                  }
                | postfix_expression {
                    $$ = $1;
                  }
                ;

multiplicative_expression: unary_expression {
                             $$ = $1;
                           }
                         | multiplicative_expression T_OP_TIMES unary_expression {
                             auto *mul_node = new ast_node_t { "*", "*", "", { $1, $3 } };
                             $$ = mul_node;
                           }
                         | multiplicative_expression T_OP_DIV unary_expression {
                             auto *div_node = new ast_node_t { "/", "/", "", { $1, $3 } };
                             $$ = div_node;
                           }
                         | multiplicative_expression T_OP_MOD unary_expression {
                             auto *mod_node = new ast_node_t { "%", "%", "", { $1, $3 } };
                             $$ = mod_node;
                           }
                         ;

additive_expression: multiplicative_expression {
                       $$ = $1;
                     }
                   | additive_expression T_OP_PLUS multiplicative_expression {
                       auto *add_node = new ast_node_t { "+", "+", "", { $1, $3 } };
                       $$ = add_node;
                     }
                   | additive_expression T_OP_MINUS multiplicative_expression {
                       auto *sub_node = new ast_node_t { "-", "-", "", { $1, $3 } };
                       $$ = sub_node;
                     }
                   ;

relational_expression: additive_expression {
                         $$ = $1;
                       }
                     | relational_expression T_OP_LT additive_expression {
                         auto *lt_node = new ast_node_t { "<", "<", "", { $1, $3 } };
                         $$ = lt_node;
                       }
                     | relational_expression T_OP_GT additive_expression {
                         auto *gt_node = new ast_node_t { ">", ">", "", { $1, $3 } };
                         $$ = gt_node;
                       }
                     | relational_expression T_OP_LTEQ additive_expression {
                         auto *lteq_node = new ast_node_t { "<=", "<=", "", { $1, $3 } };
                         $$ = lteq_node;
                       }
                     | relational_expression T_OP_GTEQ additive_expression {
                         auto *gteq_node = new ast_node_t { ">=", ">=", "", { $1, $3 } };
                         $$ = gteq_node;
                       }
                     ;

equality_expression: relational_expression {
                       $$ = $1;
                     }
                   | equality_expression T_OP_EQEQ relational_expression {
                       auto *equality_node = new ast_node_t { "==", "==", "", { $1, $3 } };
                       $$ = equality_node;
                     }
                   | equality_expression T_OP_NOTEQ relational_expression {
                       auto *not_equality_node = new ast_node_t { "!=", "!=", "", { $1, $3 } };
                       $$ = not_equality_node;
                     }
                   ;

conditional_and_expression: equality_expression {
                              $$ = $1;
                            }
                          | conditional_and_expression T_OP_AND equality_expression {
                              auto *and_node = new ast_node_t { "&&", "&&", "", { $1, $3 } };
                              $$ = and_node;
                            }
                          ;

conditional_or_expression: conditional_and_expression {
                             $$ = $1;
                           }
                         | conditional_or_expression T_OP_OR conditional_and_expression {
                             auto *or_node = new ast_node_t { "||", "||", "", { $1, $3 } };
                             $$ = or_node;
                           }
                         ;

assignment_expression: conditional_or_expression {
                         $$ = $1;
                       }
                     | assignment {
                         $$ = $1;
                       }
                     ;

assignment: identifier T_OP_EQ assignment_expression {
              $$ = new ast_node_t{ "=", "=", "", { $1, $3 } };
            }
          ;

expression: assignment_expression {
              $$ = $1;
            }
          ;

%%

void yy::Parser::error(std::string const& msg) {
	std::cerr << "Error: " << msg << "\n";
  std::cerr << "lineno: " << driver.m_lexer->lineno() << std::endl;
	exit(1);
}
