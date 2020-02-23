%{
  #include "calc.tab.h"

  // defined by bison
  // enum yytokentype {
  //   NUMBER = 258,
  //   ADD = 259,
  //   SUB = 260,
  //   MUL = 261,
  //   DIV = 262,
  //   ABS = 263,
  //   EOL = 264
  // };

  // int yylval;
  
%}

%%

[ \t] {}
"+" { return ADD; }
"-" { return SUB; }
"*" { return MUL; }
"/" { return DIV; }
"|" { return ABS; }
[0-9]+ { yylval = atoi(yytext); return NUMBER;}
\n { return EOL; }
. { printf("unknown\n"); }

%%

// int main() {
//   int tok;

//   while ((tok = yylex()) != 0) {
//     printf("%d", tok);
//     if (tok == NUMBER) {
//       printf(" = %d\n", yylval);
//     } else {
//       printf("\n");
//     }
//   }
// }