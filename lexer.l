%{
  #include <stdio.h>
  #include "y.tab.h"
%}

%no yywrap

%%

digit [0-9]
NUM {digit}+
ID ([a-zA-Z]|_)+([a-zA-Z]|_|[0-9])*
INT "int"
VOID "void"
RETURN "return"
IF "if"
ELSE "else"
WHILE "while"
RETURN "return"
WHITESPACE [ \t\v]

%%

{NUM} {return NUM;}
