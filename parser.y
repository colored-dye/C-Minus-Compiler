%{
  #include <stdio.h>
  #include <string.h>
  #include "node.h"

  #define YYDEBUG 1

  extern int yylex(void);
  extern int yyerror(const char*);
  extern int yylineno;

  char g_savedId[STRING_LENGTH];
  struct Node* g_root;
%}
%locations

%union {
  struct Node* node;
  int integer;
  float real;
}

%token ID NUM INT VOID REAL CHAR
%token RETURN IF ELSE WHILE FOR
%token <integer> GT LT GE LE NE EQ
%token <integer> ADD SUB MUL DIV
%token LP RP LB RB LC RC SEMI COMMA ASSIGN
%token ERROR
%type 
  <node> 
  program declaration_list declaration var_declaration fun_declaration
  params param compound_stmt param_list local_declarations
  statement_list statement for_stmt for_param1 for_param2 for_param3
  expression_stmt expression selection_stmt while_stmt return_stmt
  var simple_expression additive_expression term factor
  call args arg_list type_specifier relop addop mulop id

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
  type_specifier id SEMI {
    $$ = makeNode("VarDecl");
    addChild(2, $$, $1, $2);
    $$->lineno = $1->lineno;
  }
  | type_specifier id LC NUM RC SEMI {
    $$ = makeNode("VarDecl");
    addChild(3, $$, $1, $2, yylval.node);
    $$->lineno = $1->lineno;
  }
  ;

fun_declaration:
  type_specifier id LP params RP {
    $$ = makeNode("FunDecl");
    addChild(3, $$, $1, $2, $4);
    $$->lineno = $1->lineno;
  }
  | compound_stmt {
    $$ = makeNode("FunDecl");
    addChild(1, $$, $1);
    $$->lineno = $1->lineno;
  }
  ;

type_specifier:
  INT {
    $$ = makeNode("TypeSpec");
    addChild(1, $$, yylval.node);
    $$->lineno = yylval.node->lineno;
  }
  | VOID {
    $$ = makeNode("TypeSpec");
    addChild(1, $$, yylval.node);
    $$->lineno = yylval.node->lineno;
  }
  | REAL {
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
  | VOID {
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
  | param_list COMMA param {
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
  | type_specifier id LC RC {
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
  LB local_declarations statement_list RB {
    $$ = makeNode("CompountStmt");
    addChild(2, $$, $2, $3);
    $$->lineno = ($2->lineno == 0) ? $3->lineno : $2->lineno;
  }
  ;

local_declarations:
  local_declarations var_declaration {
    $$ = $1;
    addChild(1, $$, $2);
    $$->lineno = $1->lineno;
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
  expression SEMI {
    $$ = makeNode("ExprStmt");
    addChild(1, $$, $1);
    $$->lineno = $1->lineno;
  }
  | SEMI {
    $$ = makeNode("ExprStmt");
    $$->lineno = 0;
  }
  ;

expression:
  var ASSIGN expression {
    $$ = makeNode("Expr");
    addChild(2, $$, $1, $3);
    $$->lineno = $1->lineno;
  }
  | simple_expression {
    $$ = makeNode("Expr");
    addChild(1, $$, $1);
    $$->lineno = $1->lineno;
  }
  ;

selection_stmt:
  IF LP expression RP statement %prec LOWER_THAN_ELSE {
    $$ = makeNode("SelectionStmt");
    addChild(2, $$, $3, $5);
    $$->lineno = $3->lineno;
  }
  | IF LP expression RP statement ELSE statement {
    $$ = makeNode("SelectionStmt");
    addChild(3, $$, $3, $5, $7);
    $$->lineno = $3->lineno;
  }
  ;

while_stmt:
  WHILE LP expression RP statement {
    $$ = makeNode("WhileStmt");
    addChild(2, $$, $3, $5);
    $$->lineno = $3->lineno;
  }
  ;

for_stmt:
  FOR LP for_param1 SEMI for_param2 SEMI for_param3 RP statement {
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
  | var ASSIGN expression {
    $$ = makeNode("For_param1");
    addChild(2, $$, $1, $3);
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
  | var ASSIGN expression {
    $$ = makeNode("For_param3");
    addChild(2, $$, $1, $3);
    $$->lineno = $1->lineno;
  }
  ;

return_stmt:
  RETURN SEMI {
    $$ = makeNode("ReturnStmt");
    $$->lineno = yylval.integer;
  }
  | RETURN expression SEMI {
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
  | id LC expression RC {
    $$ = makeNode("Var");
    addChild(2, $$, $1, $3);
    $$->lineno = $1->lineno;
  }
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
  LT {
    $$ = makeNode("RelOp");
    addChild(1, $$, yylval.node);
    $$->lineno = $$->child->lineno;
  }
  | LE {
    $$ = makeNode("RelOp");
    addChild(1, $$, yylval.node);
    $$->lineno = $$->child->lineno;
  }
  | GT {
    $$ = makeNode("RelOp");
    addChild(1, $$, yylval.node);
    $$->lineno = $$->child->lineno;
  }
  | GE {
    $$ = makeNode("RelOp");
    addChild(1, $$, yylval.node);
    $$->lineno = $$->child->lineno;
  }
  | EQ {
    $$ = makeNode("RelOp");
    addChild(1, $$, yylval.node);
    $$->lineno = $$->child->lineno;
  }
  | NE {
    $$ = makeNode("RelOp");
    addChild(1, $$, yylval.node);
    $$->lineno = $$->child->lineno;
  }
  ;

addop:
  ADD {
    $$ = makeNode("AddOp");
    addChild(1, $$, yylval.node);
    $$->lineno = $$->child->lineno;
  }
  | SUB {
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
  LP expression RP {
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
  | NUM {
    $$ = makeNode("Factor");
    addChild(1, $$, yylval.node);
    $$->lineno = $$->child->lineno;
  }
  ;

mulop:
  MUL {
    $$ = makeNode("MulOp");
    addChild(1, $$, yylval.node);
    $$->lineno = $$->child->lineno;
  }
  | DIV {
    $$ = makeNode("MulOp");
    addChild(1, $$, yylval.node);
    $$->lineno = $$->child->lineno;
  }
  ;

call:
  id LP args RP {
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
  arg_list COMMA expression {
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
  ID {
    $$ = yylval.node;
  }
  ;

%%

int yyerror(const char *s) {
  fprintf(stderr, "%s\n", s);
}
