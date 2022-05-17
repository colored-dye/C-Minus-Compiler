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
  return MYINT;
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
  return MYVOID;
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
  return MYREAL;
  }
{CHAR} {return MYCHAR;}
{RETURN} {return MYRETURN;}
{IF} {return MYIF;}
{ELSE} {return MYELSE;}
{WHILE} {return MYWHILE;}
{FOR} {return MYFOR;}
">" {
  struct Node* tmp = makeNode(">");
  // tmp->nodeK = GT;
  tmp->is_terminal = 1;
  tmp->termKind = TermKOp;
  tmp->int_term = MYGT;
  tmp->lineno = yylineno;
  tmp->column = yylloc.first_column;
  yylval.node = tmp;
  return MYGT;
  }
"<" {
  struct Node* tmp = makeNode("<");
  // tmp->nodeK = LT;
  tmp->is_terminal = 1;
  tmp->termKind = TermKOp;
  tmp->int_term = MYLT;
  tmp->lineno = yylineno;
  tmp->column = yylloc.first_column;
  yylval.node = tmp;
  return MYLT;
  }
">=" {
  struct Node* tmp = makeNode(">=");
  // tmp->nodeK = GE;
  tmp->is_terminal = 1;
  tmp->termKind = TermKOp;
  tmp->int_term = MYGE;
  tmp->lineno = yylineno;
  tmp->column = yylloc.first_column;
  yylval.node = tmp;
  return MYGE;
  }
"<=" {
  struct Node* tmp = makeNode("<=");
  // tmp->nodeK = LE;
  tmp->is_terminal = 1;
  tmp->termKind = TermKOp;
  tmp->int_term = MYLE;
  tmp->lineno = yylineno;
  tmp->column = yylloc.first_column;
  yylval.node = tmp;
  return MYLE;
  }
"!=" {
  struct Node* tmp = makeNode("!=");
  // tmp->nodeK = NE;
  tmp->is_terminal = 1;
  tmp->termKind = TermKOp;
  tmp->int_term = MYNE;
  tmp->lineno = yylineno;
  tmp->column = yylloc.first_column;
  yylval.node = tmp;
  return MYNE;
  }
"==" {
  struct Node* tmp = makeNode("==");
  // tmp->nodeK = EQ;
  tmp->is_terminal = 1;
  tmp->termKind = TermKOp;
  tmp->int_term = MYEQ;
  tmp->lineno = yylineno;
  tmp->column = yylloc.first_column;
  yylval.node = tmp;
  return MYEQ;
  }
"(" {return MYLP;}
")" {return MYRP;}
"{" {return MYLB;}
"}" {return MYRB;}
"[" {return MYLC;}
"]" {return MYRC;}
"," {return MYCOMMA;}
";" {
  return MYSEMI;
  }
"+" {
  struct Node* tmp = makeNode("+");
  // tmp->nodeK = ADD;
  tmp->is_terminal = 1;
  tmp->termKind = TermKOp;
  tmp->int_term = MYADD;
  tmp->lineno = yylineno;
  tmp->column = yylloc.first_column;
  yylval.node = tmp;
  return MYADD;
  }
"-" {
  struct Node* tmp = makeNode("-");
  // tmp->nodeK = SUB;
  tmp->is_terminal = 1;
  tmp->termKind = TermKOp;
  tmp->int_term = MYSUB;
  tmp->lineno = yylineno;
  tmp->column = yylloc.first_column;
  yylval.node = tmp;
  return MYSUB;
  }
"*" {
  struct Node* tmp = makeNode("*");
  // tmp->nodeK = MUL;
  tmp->is_terminal = 1;
  tmp->termKind = TermKOp;
  tmp->int_term = MYMUL;
  tmp->lineno = yylineno;
  tmp->column = yylloc.first_column;
  yylval.node = tmp;
  return MYMUL;
  }
"/" {
  struct Node* tmp = makeNode("/");
  // tmp->nodeK = DIV;
  tmp->is_terminal = 1;
  tmp->termKind = TermKOp;
  tmp->int_term = MYDIV;
  tmp->lineno = yylineno;
  tmp->column = yylloc.first_column;
  yylval.node = tmp;
  return MYDIV;
  }
"=" {
  // struct Node* tmp = makeNode("=");
  // tmp->is_terminal = 1;
  // tmp->termKind = TermKOp;
  // tmp->int_term = ASSIGN;
  // tmp->lineno = yylineno;
  // tmp->column = yylloc.first_column;
  // yylval.node = tmp;
  return MYASSIGN;
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
  return MYNUM;
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
  return MYNUM;
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
  return MYNUM;
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
  return MYNUM;
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
  return MYID;
  }
{LF} { yycolumn = 1; }
{WHITESPACE} { /* Ignore whitespace */ }
. {
  fprintf(stderr, "Error at line: %d\n", yylineno);
  return MYERROR;
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
