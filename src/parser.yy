%skeleton "lalr1.cc"
%define "parser_class_name" {Parser}

%{
  #include <iostream>
  #include <string>
  #include "ASTNode.hpp"
  #include "JayCompiler.hpp"
  #include "Lexer.hpp"

  #undef yylex
  #define yylex driver.lexer->lex
%}

%parse-param { struct JayCompiler& driver }
%error-verbose

%union {
  struct ASTNode *node;
  std::vector<struct ASTNode *> *list;
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

%right '=' ';'
%left '+' '-' '*' '/' '%'
%nonassoc '!'

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

%%

program: /* empty */
     | globaldeclarations {
         std::vector<ASTNode *> nodes = *$1;
         driver.ast = new ASTNode{ Node::program, "", driver.lexer->lineno(), nodes };
       }
     ;

literal: T_NUM {
           auto num_val = std::string(driver.lexer->YYText());
           auto *num_node = new ASTNode { Node::int_t, num_val, driver.lexer->lineno(), {} };
           $$ = num_node;
         }
       | T_STR {
           auto str_val = std::string(driver.lexer->YYText());
           auto *str_node = new ASTNode { Node::string, str_val, driver.lexer->lineno(), {} };
           $$ = str_node;
         }
       | T_RESERVED_TRUE {
           auto *true_node = new ASTNode { Node::boolean_t, "true", driver.lexer->lineno(), {} };
           $$ = true_node;
         }
       | T_RESERVED_FALSE {
           auto *false_node = new ASTNode { Node::boolean_t, "false", driver.lexer->lineno(), {} };
           $$ = false_node;
         }
       ;

type: T_TYPE_INT {
        $$ = new ASTNode{ Node::int_t, "", driver.lexer->lineno(), {} };
      }
    | T_TYPE_BOOLEAN {
        $$ = new ASTNode{ Node::boolean_t, "", driver.lexer->lineno(), {} };
      }
    ;

identifier: T_ID {
              auto id_name = std::string(driver.lexer->YYText());
              $$ = new ASTNode{ Node::id, id_name, driver.lexer->lineno(), {} };
            }
          ;

globaldeclarations: globaldeclaration {
                      $$ = new std::vector<ASTNode *>();
                      $$->push_back($1);
                    }
                  | globaldeclarations globaldeclaration {
                      $$ = $1;
                      $$->push_back($2);
                    }
                  ;

globaldeclaration: variable_declaration {
                     std::vector<ASTNode *> global_var_nodes = *$1;
                     $$ = new ASTNode { Node::global_var_decl, "", driver.lexer->lineno(), global_var_nodes };
                   }
                 | function_declaration {
                   std::vector<ASTNode *> func_decl_nodes = *$1;
                   $$ = new ASTNode { Node::function_decl, "", driver.lexer->lineno(), func_decl_nodes };
                 }
                 | main_function_declaration {
                     ASTNode *main_func_node = $1;
                     $$ = main_func_node; 
                   } 
                 ;

variable_declaration: type identifier T_SEPARATOR_SEMI {
                        auto *nodes = new std::vector<ASTNode *>();

                        nodes->push_back($1);
                        nodes->push_back($2);
                        $$ = nodes;
                      }
                    ;

function_declaration: type identifier T_SEPARATOR_LPAREN T_SEPARATOR_RPAREN block {
                        auto *func_nodes = new std::vector<ASTNode *>();
                        auto *empty_formals = new ASTNode { Node::formal_params, "", 0, {} };

                        func_nodes->push_back($1);
                        func_nodes->push_back($2);
                        func_nodes->push_back(empty_formals);
                        func_nodes->push_back($5);
                        $$ = func_nodes;
                      }
                    | type identifier T_SEPARATOR_LPAREN param_list T_SEPARATOR_RPAREN block {
                        auto *func_nodes = new std::vector<ASTNode *>();

                        func_nodes->push_back($1);
                        func_nodes->push_back($2);
                        func_nodes->push_back($4);
                        func_nodes->push_back($6);
                        $$ = func_nodes;
                      }
                    | T_TYPE_VOID identifier T_SEPARATOR_LPAREN T_SEPARATOR_RPAREN block {
                        auto *void_t_node = new ASTNode { Node::void_t, "", driver.lexer->lineno(), {} };
                        auto *func_nodes = new std::vector<ASTNode *>();

                        func_nodes->push_back(void_t_node);
                        func_nodes->push_back($2);
                        func_nodes->push_back($5);
                        $$ = func_nodes;
                      }
                    | T_TYPE_VOID identifier T_SEPARATOR_LPAREN param_list T_SEPARATOR_RPAREN block {
                        auto *void_t_node = new ASTNode { Node::void_t, "", driver.lexer->lineno(), {} };
                        auto *func_nodes = new std::vector<ASTNode *>();

                        func_nodes->push_back(void_t_node);
                        func_nodes->push_back($2);
                        func_nodes->push_back($4);
                        func_nodes->push_back($6);
                        $$ = func_nodes;
                      }
                    ;

main_function_declaration: identifier T_SEPARATOR_LPAREN T_SEPARATOR_RPAREN block {
                             auto main_id = $1;
                             auto *void_t_node = new ASTNode { Node::void_t, "", driver.lexer->lineno(), {} };
                             auto *empty_formals = new ASTNode { Node::formal_params, "", 0, {} };

                             $$ = new ASTNode { Node::main_func_decl, "", driver.lexer->lineno(), { void_t_node, main_id, empty_formals, $4 } };
                           }
                         ;
param_list: param {
              auto *param_list = new ASTNode { Node::formal_params, "", 0, { $1 } };
              $$ = param_list;
            }
          | param_list T_SEPARATOR_COMMA param {
              $$ = $1;
              $$->children.push_back($3);
            }
          ;

param: type identifier {
         ASTNode *formal_param_node = new ASTNode { Node::formal, "", driver.lexer->lineno(), { $1, $2 } };
         $$ = formal_param_node;
       }
     ;

block: T_SEPARATOR_LBRACE T_SEPARATOR_RBRACE {
       $$ = new ASTNode { Node::block, "", driver.lexer->lineno(), {} };
     }
     | T_SEPARATOR_LBRACE block_statements T_SEPARATOR_RBRACE {
       auto *block_children = $2;
       auto *block = new ASTNode { Node::block, "", driver.lexer->lineno(), *block_children };

       $$ = block;
     }
     ;

block_statements: block_statement {
                    auto *block_statements = new std::vector<ASTNode *>();
                    block_statements->push_back($1);
                    $$ = block_statements;
                  }
                | block_statements block_statement {
                    $$ = $1;
                    $$->push_back($2); 
                  }
                ;

block_statement: variable_declaration {
                   auto *var_decl_node = new ASTNode{ Node::variable_decl, "", driver.lexer->lineno(), *$1 };
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
             auto *null_statement = new ASTNode { Node::null_statement, "", driver.lexer->lineno(), {} };
             $$ = null_statement;
           }
         | statement_expression T_SEPARATOR_SEMI {
             $$ = $1;
           }
         | T_RESERVED_BREAK T_SEPARATOR_SEMI {
             auto *break_statement = new ASTNode { Node::break_statement, "", driver.lexer->lineno(), {} };
             $$ = break_statement;
           }
         | T_RESERVED_RETURN expression T_SEPARATOR_SEMI {
             auto *return_statement = new ASTNode { Node::return_statement, "", driver.lexer->lineno(), { $2 } };
             $$ = return_statement;
           }
         | T_RESERVED_RETURN T_SEPARATOR_SEMI {
             auto *return_statement = new ASTNode { Node::return_statement, "", driver.lexer->lineno(), {} };
             $$ = return_statement;
           }
         | T_RESERVED_IF T_SEPARATOR_LPAREN expression T_SEPARATOR_RPAREN statement {
             auto *expression_node = $3;
             auto *statement_node = $5;
             auto *if_node = new ASTNode { Node::if_statement, "", driver.lexer->lineno(), { expression_node, statement_node } };
             $$ = if_node;
           }
         | T_RESERVED_IF T_SEPARATOR_LPAREN expression T_SEPARATOR_RPAREN statement T_RESERVED_ELSE statement {
             auto *expression_node = $3;
             auto *if_statement_node = $5;
             auto *else_statement_node = $7;
             auto *if_else_node = new ASTNode { Node::if_else_statement, "", driver.lexer->lineno(), { expression_node, if_statement_node, else_statement_node } };
             $$ = if_else_node;
           }
         | T_RESERVED_WHILE T_SEPARATOR_LPAREN expression T_SEPARATOR_RPAREN statement {
             auto *expression_node = $3;
             auto *while_statement_node = $5;
             auto *while_node = new ASTNode { Node::while_statement, "", driver.lexer->lineno(), { expression_node, while_statement_node } };
             $$ = while_node;
           }
         ;

statement_expression: assignment {
                        $$ = new ASTNode{ Node::statement_expr, "", driver.lexer->lineno(), { $1 } };
                      }
                    | function_invocation {
                        $$ = new ASTNode { Node::statement_expr, "", driver.lexer->lineno(), { $1 } };
                      }
                    ;

function_invocation: identifier T_SEPARATOR_LPAREN actuals T_SEPARATOR_RPAREN {
                       auto *actuals_node = new ASTNode { Node::actual_params, "", driver.lexer->lineno(), *$3 };
                       auto *func_call_node = new ASTNode { Node::function_call, "", driver.lexer->lineno(), { $1, actuals_node } };
                       $$ = func_call_node;
                     }
                   | identifier T_SEPARATOR_LPAREN T_SEPARATOR_RPAREN {
                       auto *actuals_node = new ASTNode { Node::actual_params, "", 0, {} };
                       auto *func_call_node = new ASTNode { Node::function_call, "", driver.lexer->lineno(), { $1, actuals_node } };
                       $$ = func_call_node;
                     }
                   ;

actuals: expression {
           auto *actuals_list = new std::vector<ASTNode *>();
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
                      node->value = node->value.insert(0, 1, '-');
                      $$ = node;
                    } else {
                      auto *unary_minus_node = new ASTNode{ Node::sub_op, "", driver.lexer->lineno(), { node } };
                      $$ = unary_minus_node;
                    }
                  }
                | T_OP_NOT unary_expression {
                    auto *not_node = new ASTNode{ Node::not_op, "", driver.lexer->lineno(), { $2 } };
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
                             auto *mul_node = new ASTNode { Node::mul_op, "", driver.lexer->lineno(), { $1, $3 } };
                             $$ = mul_node;
                           }
                         | multiplicative_expression T_OP_DIV unary_expression {
                             auto *div_node = new ASTNode { Node::div_op, "", driver.lexer->lineno(), { $1, $3 } };
                             $$ = div_node;
                           }
                         | multiplicative_expression T_OP_MOD unary_expression {
                             auto *mod_node = new ASTNode { Node::mod_op, "", driver.lexer->lineno(), { $1, $3 } };
                             $$ = mod_node;
                           }
                         ;

additive_expression: multiplicative_expression {
                       $$ = $1;
                     }
                   | additive_expression T_OP_PLUS multiplicative_expression {
                       auto *add_node = new ASTNode { Node::add_op, "", driver.lexer->lineno(), { $1, $3 } };
                       $$ = add_node;
                     }
                   | additive_expression T_OP_MINUS multiplicative_expression {
                       auto *sub_node = new ASTNode { Node::sub_op, "", driver.lexer->lineno(), { $1, $3 } };
                       $$ = sub_node;
                     }
                   ;

relational_expression: additive_expression {
                         $$ = $1;
                       }
                     | relational_expression T_OP_LT additive_expression {
                         auto *lt_node = new ASTNode { Node::lt_op, "", driver.lexer->lineno(), { $1, $3 } };
                         $$ = lt_node;
                       }
                     | relational_expression T_OP_GT additive_expression {
                         auto *gt_node = new ASTNode { Node::gt_op, "", driver.lexer->lineno(), { $1, $3 } };
                         $$ = gt_node;
                       }
                     | relational_expression T_OP_LTEQ additive_expression {
                         auto *lteq_node = new ASTNode { Node::lteq_op, "", driver.lexer->lineno(), { $1, $3 } };
                         $$ = lteq_node;
                       }
                     | relational_expression T_OP_GTEQ additive_expression {
                         auto *gteq_node = new ASTNode { Node::gteq_op,  "", driver.lexer->lineno(), { $1, $3 } };
                         $$ = gteq_node;
                       }
                     ;

equality_expression: relational_expression {
                       $$ = $1;
                     }
                   | equality_expression T_OP_EQEQ relational_expression {
                       auto *equality_node = new ASTNode { Node::eqeq_op,  "", driver.lexer->lineno(), { $1, $3 } };
                       $$ = equality_node;
                     }
                   | equality_expression T_OP_NOTEQ relational_expression {
                       auto *not_equality_node = new ASTNode { Node::noteq_op,  "", driver.lexer->lineno(), { $1, $3 } };
                       $$ = not_equality_node;
                     }
                   ;

conditional_and_expression: equality_expression {
                              $$ = $1;
                            }
                          | conditional_and_expression T_OP_AND equality_expression {
                              auto *and_node = new ASTNode { Node::bin_and_op,  "", driver.lexer->lineno(), { $1, $3 } };
                              $$ = and_node;
                            }
                          ;

conditional_or_expression: conditional_and_expression {
                             $$ = $1;
                           }
                         | conditional_or_expression T_OP_OR conditional_and_expression {
                             auto *or_node = new ASTNode { Node::bin_or_op,  "", driver.lexer->lineno(), { $1, $3 } };
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
              $$ = new ASTNode{ Node::eq_op, "", driver.lexer->lineno(), { $1, $3 } };
            }
          ;

expression: assignment_expression {
              $$ = $1;
            }
          ;

%%

void yy::Parser::error(std::string const& msg) {
  std::cerr << "filename: " << driver.filename << std::endl;
  std::cerr << "-----------------------------" << std::endl;
	std::cerr << "Error: " << msg << std::endl;
  std::cerr << "lineno: " << driver.lexer->lineno() << std::endl;
}
