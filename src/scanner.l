%option c++
%option yyclass="Lexer" 

%{
  #include <cstdio>
  #include <cstdlib>
  #include <cstring>
  #include "Lexer.hpp"

  #define TOKEN_COUNT 40

  // if there will be more than 10 warnings/error, the lexer will halt
  #define WARNING_THRESHOLD 10

  int warning_count = 0;

  // ensure that string_builder is initialized within the scanner, otherwise reading string tokens will fail
  static string_builder_t *sb = sb_init();
%}

%option yylineno
%option noyywrap
%option nounput
%option noinput

newline     (\r|\n|\r\n)
whitespace  [ \t]
input_char  [^\r\n]
string_char [^\r\n\"\\]
comment     \/\/{input_char}*
identifier  [a-zA-Z_]+[a-zA-Z0-9_]*
number      [0-9]+

%x STRING

%%

{whitespace}  {}
{comment}     {}

{newline}     {}

{number}      { return yy::Parser::token::T_NUM; }

"true"        { return yy::Parser::token::T_RESERVED_TRUE; }
"false"       { return yy::Parser::token::T_RESERVED_FALSE; }
"while"       { return yy::Parser::token::T_RESERVED_WHILE; }
"if"          { return yy::Parser::token::T_RESERVED_IF; }
"else"        { return yy::Parser::token::T_RESERVED_ELSE; }
"break"       { return yy::Parser::token::T_RESERVED_BREAK; }
"return"      { return yy::Parser::token::T_RESERVED_RETURN; }
"int"         { return yy::Parser::token::T_TYPE_INT; }
"boolean"     { return yy::Parser::token::T_TYPE_BOOLEAN; }
"void"        { return yy::Parser::token::T_TYPE_VOID; }

"("           { return yy::Parser::token::T_SEPARATOR_LPAREN; }
")"           { return yy::Parser::token::T_SEPARATOR_RPAREN; }
"{"           { return yy::Parser::token::T_SEPARATOR_LBRACE; }
"}"           { return yy::Parser::token::T_SEPARATOR_RBRACE; }
";"           { return yy::Parser::token::T_SEPARATOR_SEMI; }
","           { return yy::Parser::token::T_SEPARATOR_COMMA; }

"+"           { return yy::Parser::token::T_OP_PLUS; }
"-"           { return yy::Parser::token::T_OP_MINUS; }
"*"           { return yy::Parser::token::T_OP_TIMES; }
"/"           { return yy::Parser::token::T_OP_DIV; }
"%"           { return yy::Parser::token::T_OP_MOD; }
">"           { return yy::Parser::token::T_OP_GT; }
"<"           { return yy::Parser::token::T_OP_LT; }
">="          { return yy::Parser::token::T_OP_GTEQ; }
"<="          { return yy::Parser::token::T_OP_LTEQ; }
"="           { return yy::Parser::token::T_OP_EQ; }
"=="          { return yy::Parser::token::T_OP_EQEQ; }
"!"           { return yy::Parser::token::T_OP_NOT; }
"!="          { return yy::Parser::token::T_OP_NOTEQ; }
"&&"          { return yy::Parser::token::T_OP_AND; }
"||"          { return yy::Parser::token::T_OP_OR; }

\"  { BEGIN(STRING); }

<STRING>{

\" {
  BEGIN(INITIAL);
  if (!sb_is_empty(sb)) {
    yytext = sb_build_str(sb);
    sb_reset(sb);
  }

  return yy::Parser::token::T_STR;
}

{string_char} {
  if (YYText()[0] != '\0') {
    sb_append(sb, YYText());
  } else {
    sb_append(sb, "\00");
  }
}

\\t  { sb_append(sb, yytext); }
\\n  { sb_append(sb, yytext); }
\\r  { sb_append(sb, yytext); }
\\b  { sb_append(sb, yytext); }
\\f  { sb_append(sb, yytext); }
\\'  { sb_append(sb, yytext); }
\\\" { sb_append(sb, yytext); }
\\   { sb_append(sb, yytext); }

<<EOF>> {
  // in case if string is unterminated and there's EOF
  fprintf(stderr, "unterminated string on line %d\n", yylineno);
  BEGIN(INITIAL);
  warning_count++;
}
  
{newline} {
  fprintf(stderr, "unterminated string on line %d\n", yylineno);
  BEGIN(INITIAL);
  warning_count++;
}


}

{identifier}  { return yy::Parser::token::T_ID; }

.             {
                fprintf(stderr, "unknown char at line: %d\n", yylineno);
                if (warning_count >= WARNING_THRESHOLD) {
                  fprintf(stderr, "too many errors. aborting\n");
                  exit(EXIT_FAILURE);
                } else {
                  warning_count++;
                }
              }

%%
