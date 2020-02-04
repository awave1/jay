%{
  #include <stdio.h>
  #include <string.h>
  // has to be this weird path,
  // because the generated lexer located in `./build/generated-src/`
  #include "../../src/include/string_builder.h"

  #define TOKEN_COUNT 40

  typedef enum {
    T_EOF = 0,
    T_ID,
    T_OP,
    T_SEPARATOR,
    T_TYPE,
    T_STR,
    T_RESERVED,
    T_NUM,
    T_COMMENT,
    T_RESERVED_TRUE,
    T_RESERVED_FALSE,
    T_RESERVED_IF,
    T_RESERVED_ELSE,
    T_RESERVED_WHILE,
    T_RESERVED_BREAK,
    T_RESERVED_RETURN,
    T_TYPE_INT,
    T_TYPE_VOID,
    T_TYPE_BOOLEAN,
    T_SEPARATOR_LPAREN,
    T_SEPARATOR_RPAREN,
    T_SEPARATOR_LBRACE,
    T_SEPARATOR_RBRACE,
    T_SEPARATOR_SEMI,
    T_SEPARATOR_COMMA,
    T_OP_PLUS,
    T_OP_MINUS,
    T_OP_TIMES,
    T_OP_DIV,
    T_OP_MOD,
    T_OP_GT,
    T_OP_LT,
    T_OP_GTEQ,
    T_OP_LTEQ,
    T_OP_EQ,
    T_OP_EQEQ,
    T_OP_NOT,
    T_OP_NOTEQ,
    T_OP_AND,
    T_OP_OR,
  } token;

  typedef struct {
    token token;
    int linenum;
    char* val;
  } token_t;

  int linenum = 1;

  // Assume strings are STR_LENGTH
  char *string;
  int str_char_index = 0;

  string_builder_t *sb;
%}

%option yylineno

newline     (\r|\n|\r\n)

whitespace  [ \t]

input_char  [^(\r|\n|\r\n)]
string_char [^\r\n\"\\]


comment     \/\/{input_char}*

identifier  [a-zA-Z_]+[a-zA-Z0-9_]*

number      [0-9]*

string \"(\\.|[^"\\])*\"

%x STRING
%%

{whitespace}  {}
{comment}     {}
<<EOF>>       { return 0; }

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
  
{newline} { fprintf(stderr, "newlines in strings are not allowed\n"); }

}


{identifier}  { return T_ID; }

.             {
                fprintf(stderr, "unknown char at line: %d\n", linenum);
              }

%%

int main(int argc, char** argv) {
  if (argc != 2 || argv[1] == NULL) {
    fprintf(stderr, "Missing input file\n");
    return -1;
  }

  int token;
  sb = sb_init();

  yyin = fopen(argv[1], "r");

  char* tokens[TOKEN_COUNT];
  tokens[T_ID] = "ID";
  tokens[T_NUM] = "NUMBER";
  tokens[T_RESERVED_TRUE] = "true";
  tokens[T_RESERVED_FALSE] = "false";
  tokens[T_RESERVED_IF] = "if";
  tokens[T_RESERVED_ELSE] = "else";
  tokens[T_RESERVED_WHILE] = "while";
  tokens[T_RESERVED_BREAK] = "break";
  tokens[T_RESERVED_RETURN] = "return";
  tokens[T_TYPE_INT] = "int";
  tokens[T_TYPE_VOID] = "void";
  tokens[T_TYPE_BOOLEAN] = "boolean";
  tokens[T_SEPARATOR_LPAREN] = "(";
  tokens[T_SEPARATOR_RPAREN] = ")";
  tokens[T_SEPARATOR_LBRACE] = "{";
  tokens[T_SEPARATOR_RBRACE] = "}";
  tokens[T_SEPARATOR_SEMI] = ";";
  tokens[T_SEPARATOR_COMMA] = ",";
  tokens[T_OP_PLUS] = "+";
  tokens[T_OP_MINUS] = "-";
  tokens[T_OP_TIMES] = "*";
  tokens[T_OP_DIV] = "/";
  tokens[T_OP_MOD] = "%";
  tokens[T_OP_GT] = ">";
  tokens[T_OP_LT] = "<";
  tokens[T_OP_GTEQ] = ">=";
  tokens[T_OP_LTEQ] = "<=";
  tokens[T_OP_EQ] = "=";
  tokens[T_OP_EQEQ] = "==";
  tokens[T_OP_NOT] = "!";
  tokens[T_OP_NOTEQ] = "!=";
  tokens[T_OP_AND] = "&&";
  tokens[T_OP_OR] = "||";
  tokens[T_STR] = "string";


  while ((token = yylex()) != T_EOF) {
    printf("%d: id = %s, val = %s\n", yylineno, tokens[token], token != T_STR ? yytext : sb_build_str(sb));
    if (!sb_is_empty(sb)) {
      sb_reset(sb);
    }
  }

  sb_free(sb);
  fclose(yyin);
  return 0;
}

