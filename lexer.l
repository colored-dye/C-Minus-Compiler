%{
  #include <stdio.h>
  #include "y.tab.h"
%}

%option noyywrap

digit [0-9]
letter [a-zA-Z]
char ({letter}|"_")

INT "int"
VOID "void"
RETURN "return"
IF "if"
ELSE "else"
WHILE "while"
LR "\n"
WHITESPACE [ \t\v]+

NUM {digit}+
ID ({char})+({char}|{digit})*

%%

{INT} {return INT;}
{VOID} {return VOID;}
{RETURN} {return RETURN;}
{IF} {return IF;}
{ELSE} {return ELSE;}
{WHILE} {return WHILE;}
">" {return GT;}
"<" {return LT;}
">=" {return GE;}
"<=" {return LE;}
"!=" {return NE;}
"==" {return EQ;}
"(" {return LP;}
")" {return RP;}
"{" {return LB;}
"}" {return RB;}
"[" {return LC;}
"]" {return RC;}
"," {return COMMA;}
";" {return SEMI;}
"+" {return PLUS;}
"-" {return SUB;}
"*" {return MUL;}
"/" {return DIV;}
"=" {return ASSIGN;}
{NUM} {return NUM;}
"/*" { /* Block comment */
  while (1) {
    if (input() == '*' && input() == '/')
      break;
  }
}
"//" { /* Line comment */
  while(input() != '\n')
    ;
}
{char} {return CHAR;}
{ID} {return ID;}
{LR} { /* Ignore '\n' */ }
{WHITESPACE} { /* Ignore ' ' */ }
. {return ERROR;}

%%
