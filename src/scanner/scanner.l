%{
  #include <stdio.h>
  #include <stdlib.h>
  #include <string.h>
  #include <stdbool.h>
  #include <unistd.h>
  #include <ctype.h>
  #include <getopt.h>

  // has to be this weird path,
  // because the generated lexer located in `./build/generated-src/`
  #include "../../src/include/string_builder.h"

  #define TOKEN_COUNT 40

  // if there will be more than 10 warnings/error, the lexer will halt
  #define WARNING_THRESHOLD 10

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

  int warning_count = 0;
  string_builder_t *sb;
%}

%option yylineno

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
  
{newline} {
  fprintf(stderr, "newlines in strings are not allowed\n");
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

void print_token(char *token_name, char *token_val, bool is_compact, bool is_raw) {
  if (is_raw) {
    printf("%s\n", token_val);
    return;
  }

  if (!is_compact) {
    printf("  {\n");
    printf("    \"line\": %d,\n", yylineno);
    printf("    \"token\": \"%s\",\n", token_name);
    printf("    \"value\": \"%s\"\n", token_val);
    printf("  },\n");
  } else {
    printf("(%d, %s, %s)\n", yylineno, token_name, token_val);
  }
}

void print_help(char **argv) {
  printf("Usage: %s (-p|-r) -f <file path>\n", argv[0]);
  printf("    Tokenize the input file. By default, the output has the following format:\n");
  printf("    (linenum, token name, token value)\n\n");
  printf("    -h        prints help");
  printf("    -f <file> specify path to a file\n");
  printf("    -p        pretty print the output\n");
  printf("    -r        display tokens only. if both `-p` and `-r` are specified, `-p` is disabled\n");
}

int main(int argc, char** argv) {
  char opt;
  char *file_path = NULL;
  bool is_compact = false;
  bool is_raw = false;
  bool is_file_set = false;

  if (argc == 1) {
    print_help(argv);
    return EXIT_FAILURE;
  }

  while ((opt = getopt(argc, argv, "hcrf:")) != -1) {
    switch (opt) {
      case 'c':
        is_compact = true;
        break;
      case 'r':
        is_raw = true;
        is_compact = false;
        break;
      case 'f':
        file_path = optarg;
        is_file_set = true;
        break;
      case 'h':
        print_help(argv);
        return EXIT_SUCCESS;
      case '?':
        if (optopt == 'f') {
          fprintf (stderr, "Option -%c requires an argument.\n\n", optopt);
        } else if (isprint (optopt)) {
          fprintf (stderr, "Unknown option `-%c'.\n\n", optopt);
        } else {
          fprintf (stderr, "Unknown option character `\\x%x'.\n\n", optopt);
        }
        print_help(argv);
        return EXIT_FAILURE;
      default:
        print_help(argv); 
        exit(EXIT_FAILURE);
        break;
    }
  }

  if (!is_file_set) {
    fprintf(stderr, "file is not set\n\n");
    print_help(argv);
    exit(EXIT_FAILURE);
  }

  if ((yyin = fopen(file_path, "r")) == NULL) {
    fprintf(stderr, "failed to open a specified file '%s'\n\n", file_path);
    print_help(argv);
    return EXIT_FAILURE;
  }

  // Build the token map
  char* tokens[TOKEN_COUNT];
  tokens[T_ID] = "id";
  tokens[T_NUM] = "number";
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

  if (!is_compact) printf("[\n");

  int token;
  sb = sb_init();
  while ((token = yylex()) != T_EOF) {
    char *val = token != T_STR ? yytext : sb_build_str(sb);
    print_token(tokens[token], val, is_compact, is_raw);

    // clear out the string builder buffer
    if (token == T_STR && !sb_is_empty(sb)) {
      sb_reset(sb);
    }
  }

  if (!is_raw) printf(!is_compact ? "]\n" : "\n");

  sb_free(sb);
  fclose(yyin);
  return EXIT_SUCCESS;
}

