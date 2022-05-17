%{
  #include <stdio.h>
  #include <string.h>
  #include "node.hpp"

  #define YYDEBUG 1

  extern "C" int yylex(void);
  extern int yyerror(const char*);
  extern int yylineno;

  char g_savedId[STRING_LENGTH];
  struct Node* g_root;
  int g_syntaxError = 0;
%}
%locations

%union {
  struct Node* node;
  int integer;
  float real;
}

%token MYID MYNUM MYINT MYVOID MYREAL MYCHAR
%token MYRETURN MYIF MYELSE MYWHILE MYFOR
%token <integer> MYGT MYLT MYGE MYLE MYNE MYEQ
%token <integer> MYADD MYSUB MYMUL MYDIV
%token MYLP MYRP MYLB MYRB MYLC MYRC MYSEMI MYCOMMA MYASSIGN
%token MYERROR
%type 
  <node> 
  program declaration_list declaration var_declaration fun_declaration
  params param compound_stmt param_list local_declarations
  statement_list statement for_stmt for_param1 for_param2 for_param3
  expression_stmt expression selection_stmt while_stmt return_stmt
  var simple_expression additive_expression term factor
  call args arg_list type_specifier relop addop mulop id
  assign

%start program
%nonassoc LOWER_THAN_ELSE
%nonassoc ELSE

%%

program:
  declaration_list {
    $$ = makeNode("Program");
    addChild(1, $$, $1);
    $$->lineno = $1->lineno;
    g_root = $$;
  }
  ;

declaration_list:
  declaration {
    $$ = makeNode("GlobalDeclList");
    addChild(1, $$, $1);
    $$->lineno = $1->lineno;
  }
  | declaration_list declaration {
    $$ = $1;
    addChild(1, $$, $2);
  }
  ;

declaration:
  var_declaration {
    $$ = makeNode("Decl");
    addChild(1, $$, $1);
    $$->lineno = $1->lineno;
  }
  | fun_declaration {
    $$ = makeNode("Decl");
    addChild(1, $$, $1);
    $$->lineno = $1->lineno;
  }
  ;

var_declaration:
  type_specifier id MYSEMI {
    $$ = makeNode("VarDecl");
    addChild(2, $$, $1, $2);
    $$->lineno = $1->lineno;
  }
  | type_specifier id MYLC MYNUM MYRC MYSEMI {
    $$ = makeNode("VarDecl");
    addChild(3, $$, $1, $2, yylval.node);
    $$->lineno = $1->lineno;
  }
  ;

fun_declaration:
  type_specifier id MYLP params MYRP compound_stmt {
    $$ = makeNode("FunDecl");
    addChild(4, $$, $1, $2, $4, $6);
    $$->lineno = $1->lineno;
  }
  ;

type_specifier:
  MYINT {
    $$ = makeNode("TypeSpec");
    addChild(1, $$, yylval.node);
    $$->lineno = yylval.node->lineno;
  }
  | MYVOID {
    $$ = makeNode("TypeSpec");
    addChild(1, $$, yylval.node);
    $$->lineno = yylval.node->lineno;
  }
  | MYREAL {
    $$ = makeNode("TypeSpec");
    addChild(1, $$, yylval.node);
    $$->lineno = yylval.node->lineno;
  }
  ;

params:
  param_list {
    $$ = makeNode("Params");
    addChild(1, $$, $1);
    $$->lineno = $1->lineno;
  }
  | MYVOID {
    $$ = makeNode("Params");
    addChild(1, $$, yylval.node);
    $$->lineno = $$->child->lineno;
  }
  ;

param_list:
  param {
    $$ = makeNode("ParamList");
    addChild(1, $$, $1);
    $$->lineno = $1->lineno;
  }
  | param_list MYCOMMA param {
    $$ = $1;
    addChild(1, $$, $3);
  }
  ;

param:
  type_specifier id {
    $$ = makeNode("Param");
    addChild(2, $$, $1, $2);
    $$->lineno = $1->lineno;
  }
  | type_specifier id MYLC MYRC {
    $$ = makeNode("Param");

    struct Node* tmp = makeNode("[]");
    tmp->is_terminal = 1;
    tmp->termKind = TermKBracket;
    strncpy(tmp->str_term, "[]", STRING_LENGTH);
    addChild(3, $$, $1, $2, tmp);
    $$->lineno = $1->lineno;
  }
  ;

compound_stmt:
  MYLB local_declarations statement_list MYRB {
    $$ = makeNode("CompoundStmt");
    addChild(2, $$, $2, $3);
    $$->lineno = ($2->child) ? $2->child->lineno : $3->lineno;
  }
  ;

local_declarations:
  local_declarations var_declaration {
    $$ = $1;
    addChild(1, $$, $2);
    $$->lineno = $$->child->lineno;
  }
  | {
    $$ = makeNode("LocalDecl");
    $$->lineno = 0;
  }
  ;

statement_list:
  statement {
    $$ = makeNode("StmtList");
    addChild(1, $$, $1);
    $$->lineno = $1->lineno;
  }
  | statement_list statement {
    $$ = $1;
    addChild(1, $$, $2);
  }
  ;

statement:
  expression_stmt {
    $$ = makeNode("Stmt");
    addChild(1, $$, $1);
    $$->lineno = $1->lineno;
  }
  | compound_stmt {
    $$ = makeNode("Stmt");
    addChild(1, $$, $1);
    $$->lineno = $1->lineno;
  }
  | selection_stmt {
    $$ = makeNode("Stmt");
    addChild(1, $$, $1);
    $$->lineno = $1->lineno;
  }
  | while_stmt {
    $$ = makeNode("Stmt");
    addChild(1, $$, $1);
    $$->lineno = $1->lineno;
  }
  | for_stmt {
    $$ = makeNode("Stmt");
    addChild(1, $$, $1);
    $$->lineno = $1->lineno;
  }
  | return_stmt {
    $$ = makeNode("Stmt");
    addChild(1, $$, $1);
    $$->lineno = $1->lineno;
  }
  ;

expression_stmt:
  expression MYSEMI {
    $$ = makeNode("ExprStmt");
    addChild(1, $$, $1);
    $$->lineno = $1->lineno;
  }
  | MYSEMI {
    $$ = makeNode("ExprStmt");
    $$->lineno = yylineno;
  }
  ;

expression:
  assign {
    $$ = makeNode("Expr");
    addChild(1, $$, $1);
    $$->lineno = $1->lineno;
  }
  | simple_expression {
    $$ = makeNode("Expr");
    addChild(1, $$, $1);
    $$->lineno = $1->lineno;
  }
  ;

assign:
  var MYASSIGN expression {
    $$ = makeNode("Assign");
    addChild(2, $$, $1, $3);
    $$->lineno = $1->lineno;
  }
  ;

selection_stmt:
  MYIF MYLP expression MYRP statement %prec LOWER_THAN_ELSE {
    $$ = makeNode("SelectionStmt");
    addChild(2, $$, $3, $5);
    $$->lineno = $3->lineno;
  }
  | MYIF MYLP expression MYRP statement ELSE statement {
    $$ = makeNode("SelectionStmt");
    addChild(3, $$, $3, $5, $7);
    $$->lineno = $3->lineno;
  }
  ;

while_stmt:
  MYWHILE MYLP expression MYRP statement {
    $$ = makeNode("WhileStmt");
    addChild(2, $$, $3, $5);
    $$->lineno = $3->lineno;
  }
  ;

for_stmt:
  MYFOR MYLP for_param1 MYSEMI for_param2 MYSEMI for_param3 MYRP statement {
    $$ = makeNode("ForStmt");
    addChild(4, $$, $3, $5, $7, $9);
    $$->lineno = $3->lineno;
  }
  ;

for_param1:
  {
    $$ = makeNode("For_param1");
    $$->lineno = 0;
  }
  | assign {
    $$ = makeNode("For_param1");
    addChild(1, $$, $1);
    $$->lineno = $1->lineno;
  }
  ;

for_param2:
  {
    $$ = makeNode("For_param2");
    $$->lineno = 0;
  }
  | expression {
    $$ = makeNode("For_param2");
    addChild(1, $$, $1);
    $$->lineno = $1->lineno;
  }
  ;

for_param3:
  {
    $$ = makeNode("For_param3");
    $$->lineno = 0;
  }
  | assign {
    $$ = makeNode("For_param3");
    addChild(1, $$, $1);
    $$->lineno = $1->lineno;
  }
  ;

return_stmt:
  MYRETURN MYSEMI {
    $$ = makeNode("ReturnStmt");
    $$->lineno = yylval.integer;
  }
  | MYRETURN expression MYSEMI {
    $$ = makeNode("ReturnStmt");
    addChild(1, $$, $2);
    $$->lineno = $2->lineno;
  }
  ;

var:
  id {
    $$ = makeNode("Var");
    addChild(1, $$, $1);
    $$->lineno = $1->lineno;
  }
  | id MYLC expression MYRC {
    $$ = makeNode("Var");
    addChild(2, $$, $1, $3);
    $$->lineno = $1->lineno;
  }
  /* | error {
    fprintf(stderr, "Error at line %d: `%s' is not a variable\n", yylval.node->lineno, yylval.node->name);
    g_syntaxError++;
  } */
  ;

simple_expression:
  additive_expression relop additive_expression {
    $$ = makeNode("SimpleExpr");
    addChild(3, $$, $1, $2, $3);
    $$->lineno = $1->lineno;
  }
  | additive_expression {
    $$ = makeNode("SimpleExpr");
    addChild(1, $$, $1);
    $$->lineno = $1->lineno;
  }
  ;

additive_expression:
  additive_expression addop term {
    $$ = makeNode("AddExpr");
    addChild(3, $$, $1, $2, $3);
    $$->lineno = $1->lineno;
  }
  | term {
    $$ = makeNode("AddExpr");
    addChild(1, $$, $1);
    $$->lineno = $1->lineno;
  }
  ;

relop:
  MYLT {
    $$ = makeNode("RelOp");
    addChild(1, $$, yylval.node);
    $$->lineno = $$->child->lineno;
  }
  | MYLE {
    $$ = makeNode("RelOp");
    addChild(1, $$, yylval.node);
    $$->lineno = $$->child->lineno;
  }
  | MYGT {
    $$ = makeNode("RelOp");
    addChild(1, $$, yylval.node);
    $$->lineno = $$->child->lineno;
  }
  | MYGE {
    $$ = makeNode("RelOp");
    addChild(1, $$, yylval.node);
    $$->lineno = $$->child->lineno;
  }
  | MYEQ {
    $$ = makeNode("RelOp");
    addChild(1, $$, yylval.node);
    $$->lineno = $$->child->lineno;
  }
  | MYNE {
    $$ = makeNode("RelOp");
    addChild(1, $$, yylval.node);
    $$->lineno = $$->child->lineno;
  }
  ;

addop:
  MYADD {
    $$ = makeNode("AddOp");
    addChild(1, $$, yylval.node);
    $$->lineno = $$->child->lineno;
  }
  | MYSUB {
    $$ = makeNode("AddOp");
    addChild(1, $$, yylval.node);
    $$->lineno = $$->child->lineno;
  }
  ;

term:
  term mulop factor {
    $$ = makeNode("Term");
    addChild(3, $$, $1, $2, $3);
    $$->lineno = $1->lineno;
  }
  | factor {
    $$ = makeNode("Term");
    addChild(1, $$, $1);
    $$->lineno = $1->lineno;
  }
  ;

factor:
  MYLP expression MYRP {
    $$ = makeNode("Factor");
    addChild(1, $$, $2);
    $$->lineno = $2->lineno;
  }
  | var {
    $$ = makeNode("Factor");
    addChild(1, $$, $1);
    $$->lineno = $1->lineno;
  }
  | call {
    $$ = makeNode("Factor");
    addChild(1, $$, $1);
    $$->lineno = $1->lineno;
  }
  | MYNUM {
    $$ = makeNode("Factor");
    addChild(1, $$, yylval.node);
    $$->lineno = $$->child->lineno;
  }
  ;

mulop:
  MYMUL {
    $$ = makeNode("MulOp");
    addChild(1, $$, yylval.node);
    $$->lineno = $$->child->lineno;
  }
  | MYDIV {
    $$ = makeNode("MulOp");
    addChild(1, $$, yylval.node);
    $$->lineno = $$->child->lineno;
  }
  ;

call:
  id MYLP args MYRP {
    $$ = makeNode("Call");
    addChild(2, $$, $1, $3);
    $$->lineno = $1->lineno;
  }
  ;

args:
  arg_list {
    $$ = makeNode("Args");
    addChild(1, $$, $1);
    $$->lineno = $1->lineno;
  }
  | {
    $$ = makeNode("Args");
    $$->lineno = 0;
  }
  ;

arg_list:
  arg_list MYCOMMA expression {
    $$ = $1;
    addChild(1, $$, $3);
    $$->lineno = $1->lineno;
  }
  | expression {
    $$ = makeNode("ArgList");
    addChild(1, $$, $1);
    $$->lineno = $1->lineno;
  }
  ;

id:
  MYID {
    $$ = yylval.node;
  }
  | error {
    fprintf(stderr, "Error at line %d: `%s' is not a valid identifier.\n", yylval.node->lineno, yylval.node->name);
    g_syntaxError++;
  }
  ;

%%

int yyerror(const char *s) {
  fprintf(stderr, "%s\n", s);
}
