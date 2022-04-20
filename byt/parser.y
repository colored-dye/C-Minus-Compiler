%{
  #include <stdio.h>
  #include "node.h"

  extern char *yytext;
  extern FILE *yyin;
  #define YYDEBUG 1
%}

%union {
  struct treeNode* node;
  int i;
}

%token NUM ID INT VOID CHAR
%token RETURN IF ELSE WHILE
%token GT LT GE LE NE EQ
%token LP RP LB RB LC RC SEMI COMMA
%token PLUS SUB MUL DIV ASSIGN
%token ERROR
%type <node> program declaration_list declaration var_declaration fun_declaration
  type_specifier params param compound_stmt param_list local_declarations
  statement_list statement empty
  expression_stmt expression selection_stmt iteration_stmt return_stmt
  var simple_expression additive_expression relop addop term factor
  mulop call args arg_list

%start program
%nonassoc LOWER_THAN_ELSE
%nonassoc ELSE

%%

program:
  declaration_list
  ;

declaration_list:
  declaration
  | declaration_list declaration
  ;

declaration:
  var_declaration
  | fun_declaration
  ;

var_declaration:
  type_specifier ID SEMI
  | type_specifier ID LC NUM RC
  ;

fun_declaration:
  type_specifier ID LP params RP
  | compound_stmt
  ;

type_specifier:
  INT
  | VOID
  ;

params:
  param_list
  | VOID
  ;

param_list:
  param
  | param_list COMMA param
  ;

param:
  type_specifier ID
  | type_specifier ID LC RC
  ;

compound_stmt:
  LB local_declarations statement_list RB
  ;

local_declarations:
  local_declarations var_declaration
  | empty
  ;

statement_list:
  ;

statement:
  ;

empty: { /* Empty string */
  $$ = NULL;
}
  ;

expression_stmt:
  expression SEMI
  | SEMI
  ;

expression:
  var ASSIGN expression
  | simple_expression
  ;

selection_stmt:
  IF LP expression RP statement %prec LOWER_THAN_ELSE
  | IF LP expression RP statement ELSE statement
  ;

iteration_stmt:
  WHILE LP expression RP statement
  ;

return_stmt:
  RETURN SEMI
  | RETURN expression SEMI
  ;

var:
  ID
  | ID LC expression RC
  ;

simple_expression:
  additive_expression relop additive_expression
  | additive_expression
  ;

additive_expression:
  additive_expression addop term
  | term
  ;

relop:
  LT { $$ = $1; }
  | LE { $$ = $1; }
  | GT { $$ = $1; }
  | GE { $$ = $1; }
  | EQ { $$ = $1; }
  | NE { $$ = $1; }
  ;

addop:
  PLUS { $$ = $1; }
  | SUB { $$ = $1; }
  ;

term:
  term mulop factor
  | factor
  ;

factor:
  LP expression RP
  | var
  | call
  | NUM {
    int i = atoi(yytext);
    return i;
  }
  ;

mulop:
  MUL { $$ = $1; }
  | DIV { $$ = $1; }
  ;

call:
  ID LP args RP
  ;

args:
  arg_list
  | empty
  ;

arg_list:
  arg_list COMMA expression
  | expression
  ;

%%

int yyerror(const char *s) {
  fprintf(stderr, "%s\n", s);
}
