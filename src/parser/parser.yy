%{
#include <stdio.h>
#include <stdlib.h>
%}

%%

statement: NAME '=' expression
          ;

expression:   NUMBER '+' NUMBER
            | NUMBER '-' NUMBER
            ;

%%
