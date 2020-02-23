%{
  #include <stdio.h>
  #include <stdlib.h>
  #include <string.h>
  #include <stdbool.h>
  #include <ctype.h>
  #include "parser.tab.h"

  // has to be this weird path,
  // because the generated lexer located in `./build/generated-src/`
  #include "../../src/include/string_builder.h"

  #define TOKEN_COUNT 40

  // if there will be more than 10 warnings/error, the lexer will halt
  #define WARNING_THRESHOLD 10

  int warning_count = 0;
  string_builder_t *sb;
%}

%option yylineno
%option nounput
%option noinput

newline     (\r|\n|\r\n)
whitespace  [ \t]
input_char  [^(\r|\n|\r\n)]
string_char [^\r\n\"\\]
comment     \/\/{input_char}*
identifier  [a-zA-Z_]+[a-zA-Z0-9_]*
number      [0-9]+

%x STRING

%%

{whitespace}  {}
{comment}     {}

{newline}     {}

{number}      { return T_NUM; }

"true"        { return T_RESERVED_TRUE; }
"false"       { return T_RESERVED_FALSE; }
"while"       { return T_RESERVED_WHILE; }
"if"          { return T_RESERVED_IF; }
"else"        { return T_RESERVED_ELSE; }
"break"       { return T_RESERVED_BREAK; }
"return"      { return T_RESERVED_RETURN; }
"int"         { return T_TYPE_INT; }
"boolean"     { return T_TYPE_BOOLEAN; }
"void"        { return T_TYPE_VOID; }

"("           { return T_SEPARATOR_LPAREN; }
")"           { return T_SEPARATOR_RPAREN; }
"{"           { return T_SEPARATOR_LBRACE; }
"}"           { return T_SEPARATOR_RBRACE; }
";"           { return T_SEPARATOR_SEMI; }
","           { return T_SEPARATOR_COMMA; }

"+"           { return T_OP_PLUS; }
"-"           { return T_OP_MINUS; }
"*"           { return T_OP_TIMES; }
"/"           { return T_OP_DIV; }
"%"           { return T_OP_MOD; }
">"           { return T_OP_GT; }
"<"           { return T_OP_LT; }
">="          { return T_OP_GTEQ; }
"<="          { return T_OP_LTEQ; }
"="           { return T_OP_EQ; }
"=="          { return T_OP_EQEQ; }
"!"           { return T_OP_NOT; }
"!="          { return T_OP_NOTEQ; }
"&&"          { return T_OP_AND; }
"||"          { return T_OP_OR; }

\"  { BEGIN(STRING); }

<STRING>{

\" {
  BEGIN(INITIAL);
  return T_STR;
}

{string_char} {
  if (yytext[0] != '\0') {
    sb_append(sb, yytext);
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

{identifier}  { return T_ID; }

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
