%{
  #include <stdio.h>
  #include "y.tab.h"
  #include "node.hpp"
  extern YYSTYPE yylval;
  int yycolumn = 1;

  #define YY_USER_ACTION \
        yylloc.first_line = yylloc.last_line = yylineno; \
        yylloc.first_column = yycolumn;  \
        yylloc.last_column = yycolumn + yyleng - 1; \
        yycolumn += yyleng;
  int my_atoi(const char *s, int base);
  extern char g_savedId[STRING_LENGTH];
  extern "C" int yylex(void);
%}

%option noyywrap
%option yylineno

digit [0-9]
digits [0-9]+
letter [a-zA-Z]
char ({letter}|_)

INT "int"
VOID "void"
REAL "real"
CHAR "char"
RETURN "return"
IF "if"
ELSE "else"
WHILE "while"
FOR "for"
LF "\n"
WHITESPACE [ \t\v]+

OCT             0[0-7]+
/* OCTERROR        0[0-7]*[8-9]+[0-9]* */
HEX             0[xX][0-9a-fA-F]+
/* HEXERROR        0[xX][0-9a-fA-F]*[g-zG-Z]+[0-9a-zA-Z]* */
DEC             0|([1-9][0-9]*)
/* DECERROR        [0-9]+[a-wA-Wy-zY-Z]+[0-9a-dA-Df-zF-Z]*|[0-9]+[0-9]+[a-dA-Df-zF-Z]+[0-9]*[0-9a-dA-Df-zF-Z]* */
FLOAT           {digits}\.{digits}|(\.{digits}|{digits}\.|{digits}\.{digits}|{digits})([eE][+-]?{digits})
/* FLOATERROR      (\.{digits}|{digits}\.|{digits}\.{digits})([eE][+-]?) */
ID ({char})+({char}|{digit})*

%%

{INT} {
  struct Node* tmp = makeNode("INT");
  // tmp->nodeK = INT;
  tmp->is_terminal = 1;
  tmp->termKind = TermKType;
  strncpy(tmp->str_term, "INT", STRING_LENGTH);
  tmp->lineno = yylineno;
  tmp->column = yylloc.first_column;
  yylval.node = tmp;
  return INT;
  }
{VOID} {
  struct Node* tmp = makeNode("VOID");
  // tmp->nodeK = VOID;
  tmp->is_terminal = 1;
  tmp->termKind = TermKType;
  strncpy(tmp->str_term, "VOID", STRING_LENGTH);
  tmp->lineno = yylineno;
  tmp->column = yylloc.first_column;
  yylval.node = tmp;
  return VOID;
  }
{REAL} {
  struct Node* tmp = makeNode("REAL");
  // tmp->nodeK = REAL;
  tmp->is_terminal = 1;
  tmp->termKind = TermKType;
  strncpy(tmp->str_term, "REAL", STRING_LENGTH);
  tmp->lineno = yylineno;
  tmp->column = yylloc.first_column;
  yylval.node = tmp;
  return REAL;
  }
{CHAR} {return CHAR;}
{RETURN} {return RETURN;}
{IF} {return IF;}
{ELSE} {return ELSE;}
{WHILE} {return WHILE;}
{FOR} {return FOR;}
">" {
  struct Node* tmp = makeNode(">");
  // tmp->nodeK = GT;
  tmp->is_terminal = 1;
  tmp->termKind = TermKOp;
  tmp->int_term = GT;
  tmp->lineno = yylineno;
  tmp->column = yylloc.first_column;
  yylval.node = tmp;
  return GT;
  }
"<" {
  struct Node* tmp = makeNode("<");
  // tmp->nodeK = LT;
  tmp->is_terminal = 1;
  tmp->termKind = TermKOp;
  tmp->int_term = LT;
  tmp->lineno = yylineno;
  tmp->column = yylloc.first_column;
  yylval.node = tmp;
  return LT;
  }
">=" {
  struct Node* tmp = makeNode(">=");
  // tmp->nodeK = GE;
  tmp->is_terminal = 1;
  tmp->termKind = TermKOp;
  tmp->int_term = GE;
  tmp->lineno = yylineno;
  tmp->column = yylloc.first_column;
  yylval.node = tmp;
  return GE;
  }
"<=" {
  struct Node* tmp = makeNode("<=");
  // tmp->nodeK = LE;
  tmp->is_terminal = 1;
  tmp->termKind = TermKOp;
  tmp->int_term = LE;
  tmp->lineno = yylineno;
  tmp->column = yylloc.first_column;
  yylval.node = tmp;
  return LE;
  }
"!=" {
  struct Node* tmp = makeNode("!=");
  // tmp->nodeK = NE;
  tmp->is_terminal = 1;
  tmp->termKind = TermKOp;
  tmp->int_term = NE;
  tmp->lineno = yylineno;
  tmp->column = yylloc.first_column;
  yylval.node = tmp;
  return NE;
  }
"==" {
  struct Node* tmp = makeNode("==");
  // tmp->nodeK = EQ;
  tmp->is_terminal = 1;
  tmp->termKind = TermKOp;
  tmp->int_term = EQ;
  tmp->lineno = yylineno;
  tmp->column = yylloc.first_column;
  yylval.node = tmp;
  return EQ;
  }
"(" {return LP;}
")" {return RP;}
"{" {return LB;}
"}" {return RB;}
"[" {return LC;}
"]" {return RC;}
"," {return COMMA;}
";" {
  return SEMI;
  }
"+" {
  struct Node* tmp = makeNode("+");
  // tmp->nodeK = ADD;
  tmp->is_terminal = 1;
  tmp->termKind = TermKOp;
  tmp->int_term = ADD;
  tmp->lineno = yylineno;
  tmp->column = yylloc.first_column;
  yylval.node = tmp;
  return ADD;
  }
"-" {
  struct Node* tmp = makeNode("-");
  // tmp->nodeK = SUB;
  tmp->is_terminal = 1;
  tmp->termKind = TermKOp;
  tmp->int_term = SUB;
  tmp->lineno = yylineno;
  tmp->column = yylloc.first_column;
  yylval.node = tmp;
  return SUB;
  }
"*" {
  struct Node* tmp = makeNode("*");
  // tmp->nodeK = MUL;
  tmp->is_terminal = 1;
  tmp->termKind = TermKOp;
  tmp->int_term = MUL;
  tmp->lineno = yylineno;
  tmp->column = yylloc.first_column;
  yylval.node = tmp;
  return MUL;
  }
"/" {
  struct Node* tmp = makeNode("/");
  // tmp->nodeK = DIV;
  tmp->is_terminal = 1;
  tmp->termKind = TermKOp;
  tmp->int_term = DIV;
  tmp->lineno = yylineno;
  tmp->column = yylloc.first_column;
  yylval.node = tmp;
  return DIV;
  }
"=" {
  // struct Node* tmp = makeNode("=");
  // tmp->is_terminal = 1;
  // tmp->termKind = TermKOp;
  // tmp->int_term = ASSIGN;
  // tmp->lineno = yylineno;
  // tmp->column = yylloc.first_column;
  // yylval.node = tmp;
  return ASSIGN;
  }
{DEC} {
  struct Node* node = makeNode(yytext);
  // node->nodeK = NUM;
  node->is_terminal = 1;
  node->termKind = TermKNum;
  node->is_int = 1;
  node->int_term = atoi(yytext);
  node->column = yylloc.first_column;
  node->lineno = yylineno;
  yylval.node = node;
  return NUM;
}
{OCT} {
  struct Node* node = makeNode(yytext);
  // node->nodeK = NUM;
  node->is_terminal = 1;
  node->termKind = TermKNum;
  node->is_int = 1;
  node->int_term = my_atoi(yytext, 8);
  node->column = yylloc.first_column;
  node->lineno = yylineno;
  yylval.node = node;
  return NUM;
}
{HEX} {
  struct Node* node = makeNode(yytext);
  // node->nodeK = NUM;
  node->is_terminal = 1;
  node->termKind = TermKNum;
  node->is_int = 1;
  node->int_term = my_atoi(yytext, 16);
  node->column = yylloc.first_column;
  node->lineno = yylineno;
  yylval.node = node;
  return NUM;
}
{FLOAT} {
  struct Node* node = makeNode(yytext);
  // node->nodeK = NUM;
  node->is_terminal = 1;
  node->termKind = TermKNum;
  node->is_int = 0;
  node->real_term = atof(yytext);
  node->column = yylloc.first_column;
  node->lineno = yylineno;
  yylval.node = node;
  return NUM;
}
\/\*(.*\n)*.*\*\/ { /* Block comment */
  // while (1) {
  //   if (input() == '*' && input() == '/')
  //     break;
  // }
}
\/\/.* { /* Line comment */
  // char c;
  // while((c = input()) != 0 && c != '\n')
  //   ;
}
{ID} {
  struct Node* id = makeNode(yytext);
  // id->nodeK = ID;
  id->is_terminal = 1;
  id->termKind = TermKId;
  strncpy(id->str_term, yytext, STRING_LENGTH);
  id->column = yylloc.first_column;
  id->lineno = yylineno;
  yylval.node = id;
  return ID;
  }
{LF} { yycolumn = 1; }
{WHITESPACE} { /* Ignore whitespace */ }
. {
  fprintf(stderr, "Error at line: %d\n", yylineno);
  return ERROR;
  }

%%

int my_atoi(const char *s, int base) {
  int result = 0;
  int i;
  for(i=0; s[i]; i++) {
    int t;
    if(s[i] >= '0' && s[i] <= '9')
      t = s[i] - '0';
    else if(s[i] >= 'a' && s[i] <= 'f')
      t = s[i] - 'a' + 10;
    else if(s[i] >= 'A' && s[i] <= 'F')
      t = s[i] - 'A' + 10;
    result = result * base + t;
  }
  return result;
}
