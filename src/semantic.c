#include "semantic.h"
#include "node.h"
#include "symtable.h"
#include <stdio.h>
#include <string.h>

struct Type g_FuncReturnType;
const char* g_BuiltinFunction[2] = {
  "input", "output",
};
struct Node *g_SENode1, *g_SENode2;
struct Type *g_SEType1, *g_SEType2;

void SemanticError(int lineno, int column, SemanticErrorKind errK) {
  fprintf(stderr, "Error at line %d: ", lineno);
  const int speclen = 256;
  char spec[speclen];
  const char *type1, *type2;

  switch(errK) {
  case SEReturnType:
    type1 = Type2Str(g_SEType1);
    type2 = Type2Str(g_SEType2);
    snprintf(spec, speclen, "Return type `%s' does not match `%s' in function: `%s'.", type1, type2, g_SENode1->str_term);
    break;
  case SEConditionNotNum:
    snprintf(spec, speclen, "Condition value should be `int' or `real'.");
    break;
  case SEAssignType:
    type1 = Type2Str(g_SEType1);
    type2 = Type2Str(g_SEType2);
    snprintf(spec, speclen, "Assign type not match: `%s' = `%s'.", type1, type2);
    break;
  case SEUsedBeforeDecl:
    snprintf(spec, speclen, "`%s' used before definition.", g_SENode1->str_term);
    break;
  case SEInvalidIndex:
    snprintf(spec, speclen, "Index to array not int.");
    break;
  case SENotArray:
    snprintf(spec, speclen, "`%s' is not an array.", g_SENode1->str_term);
    break;
  case SEOpTypeNotMatch:
    type1 = Type2Str(g_SEType1);
    type2 = Type2Str(g_SEType2);
    snprintf(spec, speclen, "Operand type `%s' and `%s' of operator `%s' do not match.", type1, type2, g_SENode1->name);
    break;
  case SEFuncArgsNotMatch:
    snprintf(spec, speclen, "Wrong arguments for function `%s'.", g_SENode1->str_term);
    break;
  // default:
  //   break;
  }
  fprintf(stderr, "%s\n", spec);
}

void SemanticAnalysis(struct Node* root) {
  #ifdef SEMANTIC_DEBUG
  puts("Semantic Analysis Started");
  #endif

  Program(root->child);

  #ifdef SEMANTIC_DEBUG
  puts("Semantic Analysis Finished");
  #endif
}

void Program(struct Node* node) {
  #ifdef SEMANTIC_DEBUG
  printf("Program(%d)\n", node->lineno);
  #endif

  // Prepare builtin functions
  // int input(void)
  struct SymNode* builtin = createSymNode("input");
  builtin->symType = createType(FuncK);
  builtin->symType->func = createFuncArg("");
  builtin->symType->func->argType = createType(BasicK);
  builtin->symType->func->argType->basic = Int; // Return: int
  builtin->symType->func->nextArg = createFuncArg("");
  builtin->symType->func->nextArg->argType = createType(BasicK);
  builtin->symType->func->nextArg->argType->basic = Void; // Arg: void
  insert(builtin);

  // void output(int)
  builtin = createSymNode("output");
  builtin->symType = createType(FuncK);
  builtin->symType->func = createFuncArg("");
  builtin->symType->func->argType = createType(BasicK);
  builtin->symType->func->argType->basic = Void; // Return: void
  builtin->symType->func->nextArg = createFuncArg("");
  builtin->symType->func->nextArg->argType = createType(BasicK);
  builtin->symType->func->nextArg->argType->basic = Int; // Arg: Int
  insert(builtin);
  
  DeclList(node->child);
}

void DeclList(struct Node* node) {
  // declaration_list -> declaration | declaration_list declaration
  #ifdef SEMANTIC_DEBUG
  printf("DeclList(%d)\n", node->lineno);
  #endif
  struct Node* p = node;
  while(p) {
    Decl(p);
    p = p->next_sib;
  }
}

void Decl(struct Node* node) {
  // declaration -> var_declaration | fun_declaration
  #ifdef SEMANTIC_DEBUG
  printf("Declaration(%d)\n", node->lineno);
  #endif
  node = node->child;
  if(!strncmp(node->name, "VarDecl", NAME_LENGTH)) {
    VarDecl(node);
  } else if(!strncmp(node->name, "FunDecl", NAME_LENGTH)) {
    FuncDecl(node);
  } else {
    fprintf(stderr, "Decl(): Unrecognized node.\n");
  }
}

void VarDecl(struct Node* node) {
  // var_declaration -> type_specifier id | type_specifier id [NUM]
  #ifdef SEMANTIC_DEBUG
  printf("VarDecl(%d)\n", node->lineno);
  #endif
  struct Type* type = TypeSpec(node->child);
  if(node->child->next_sib->next_sib) {
    // 如果id后还有参数，就是array
    struct Type* array = createType(ArrayK);
    array->array.arrType = type;
    array->array.size = node->child->next_sib->next_sib->int_term;
    type = array;
  }
  struct SymNode* sym = createSymNode(node->child->next_sib->str_term);
  sym->symType = type;
  insert(sym);
}

struct Type* TypeSpec(struct Node* node) {
  // type_specifier -> INT | VOID | REAL
  #ifdef SEMANTIC_DEBUG
  printf("TypeSpec(%d)\n", node->lineno);
  #endif
  struct Type* type = createType(BasicK);
  setBasic(type, str2BasicType(node->child->str_term));
  return type;
}

void FuncDecl(struct Node* node) {
  // fun_declaration -> type_specifier id (params) compound_stmt
  #ifdef SEMANTIC_DEBUG
  printf("FuncDecl(%d)\n", node->lineno);
  #endif
  struct Type* type, *rettype = TypeSpec(node->child);
  // Store function return type for ReturnStmt to check
  g_FuncReturnType = *rettype;
  char* id = node->child->next_sib->str_term;
  struct FuncArgList* params = Params(node->child->next_sib->next_sib);
  
  type = createType(FuncK);
  type->func = createFuncArg(id);
  type->func->argType = rettype;
  type->func->nextArg = params;
  
  struct SymNode* sym = createSymNode(id);
  sym->symType = type;
  insert(sym);

  // Check compound_stmt
  CompoundStmt(node->child->next_sib->next_sib->next_sib);
}

struct FuncArgList* Params(struct Node* node) {
  // params -> param_list | e
  #ifdef SEMANTIC_DEBUG
  printf("Params(%d)\n", node->lineno);
  #endif
  return ParamList(node->child);
}

struct FuncArgList* ParamList(struct Node* node) {
  // param_list -> param | param_list param
  #ifdef SEMANTIC_DEBUG
  printf("ParamList(%d)\n", node->lineno);
  #endif
  struct Node* p = node->child;
  struct FuncArgList* head = NULL, *tail = NULL;
  while(p) {
    if(head == NULL) {
      head = tail = Param(p);
    } else {
      tail->nextArg = Param(p);
      tail = tail->nextArg;
    }
    p = p->next_sib;
  }
  return head;
}

struct FuncArgList* Param(struct Node* node) {
  // param -> type_specifier id | type_specifier id []
  #ifdef SEMANTIC_DEBUG
  printf("Param(%d)\n", node->lineno);
  #endif
  struct FuncArgList* arg = createFuncArg(node->name);
  struct Type* type = createType(BasicK);
  setBasic(type, str2BasicType(node->str_term));
  if(node->child->next_sib->next_sib) {
    // Array
    struct Type* array = createType(ArrayK);
    array->array.arrType = type;
    array->array.size = -1;
    type = array;
  }
  arg->argType = type;

  return arg;
}

void CompoundStmt(struct Node* node) {
  // compound_stmt -> { local_declarations statemene_list }
  #ifdef SEMANTIC_DEBUG
  printf("CompoundStmt(%d)\n", node->lineno);
  #endif
  LocalDecl(node->child);
  StmtList(node->child->next_sib);
}

void LocalDecl(struct Node* node) {
  // local_declarations -> local_declarations var_declaration | e
  #ifdef SEMANTIC_DEBUG
  printf("LocalDecl(%d)\n", node->lineno);
  #endif
  struct Node* p = node->child;
  while(p) {
    VarDecl(p);
    p = p->next_sib;
  }
}

void StmtList(struct Node* node) {
  // statement_list -> statement | statement_list statement
  #ifdef SEMANTIC_DEBUG
  printf("StmtList(%d)\n", node->lineno);
  #endif
  node = node->child;
  while(node) {
    Stmt(node);
    node = node->next_sib;
  }
}

void Stmt(struct Node* node) {
  // statement -> expression_stmt | compound_stmt | selection_stmt 
  //            | while_stmt | for_stmt | return_stmt
  #ifdef SEMANTIC_DEBUG
  printf("Stmt(%d)\n", node->lineno);
  #endif
  node = node->child;
  if(!node) {
    // Error
  }
  switch(node->name[0]) {
  case 'E': // ExprStmt
    ExprStmt(node); break;
  case 'C': // CompoundStmt
    CompoundStmt(node); break;
  case 'S': // SelectionStmt
    SelectionStmt(node); break;
  case 'W': // WhileStmt
    WhileStmt(node); break;
  case 'F': // ForStmt
    ForStmt(node); break;
  case 'R': // ReturnStmt
    ReturnStmt(node); break;
  default:
    break;
  }
}

void ExprStmt(struct Node* node) {
  // expression_stmt -> expression | e
  #ifdef SEMANTIC_DEBUG
  printf("ExprStmt(%d)\n", node->lineno);
  #endif
  node = node->child;
  if(!node)
    return;
  Expr(node);
}

void SelectionStmt(struct Node* node) {
  // selection_stmt -> IF ( expression ) statement |
  //                 | IF ( expression ) statement ELSE statement
  #ifdef SEMANTIC_DEBUG
  printf("SelectionStmt(%d)\n", node->lineno);
  #endif
  struct Type* exprType = Expr(node->child);
  if(exprType->typeKind != BasicK || exprType->basic != Int || exprType->basic != Real) {
    SemanticError(node->child->lineno, node->child->column, SEConditionNotNum);
  }
  Stmt(node->child->next_sib);
  if(node->child->next_sib->next_sib) {
    Stmt(node->child->next_sib->next_sib);
  }
}

void WhileStmt(struct Node* node) {
  // while_stmt -> WHILE ( expression ) statement
  #ifdef SEMANTIC_DEBUG
  printf("WhileStmt(%d)\n", node->lineno);
  #endif
  struct Type* exprType = Expr(node->child);
  if(exprType->typeKind != BasicK || exprType->basic != Int || exprType->basic != Real) {
    SemanticError(node->child->lineno, node->child->column, SEConditionNotNum);
  }
  Stmt(node->child->next_sib);
}

void ForStmt(struct Node* node) {
  // for_stmt -> FOR ( for_param1; for_param2; for_param3 ) statement
  // for_param1 -> e | var ASSIGN expression
  // for_param2 -> e | expression
  // for_param3 -> e | var ASSIGN expression
  #ifdef SEMANTIC_DEBUG
  printf("ForStmt(%d)\n", node->lineno);
  #endif
  node = node->child;
  if(node->child) {
    Assign(node->child);
  }
  node = node->next_sib;
  if(node->child) {
    Expr(node->child);
  }
  node = node->next_sib;
  if(node->child) {
    Assign(node->child);
  }
}

void ReturnStmt(struct Node* node) {
  // return_stmt -> RETURN | RETURN expression
  #ifdef SEMANTIC_DEBUG
  printf("ReturnStmt(%d)\n", node->lineno);
  #endif
  if(!node->child) {
    // No return value indicates void
    if(g_FuncReturnType.typeKind != BasicK || g_FuncReturnType.basic != Void) {
      g_SEType2 = &g_FuncReturnType;
      g_SEType1 = createType(BasicK);
      setBasic(g_SEType1, Void);
      SemanticError(node->lineno, node->column, SEReturnType);
    }
  } else {
    struct Type* retType = Expr(node->child);
    int match = 1;
    if(!isTypeMatch(retType, &g_FuncReturnType)) {
      g_SEType2 = &g_FuncReturnType;
      g_SEType1 = retType;
      SemanticError(node->lineno, node->column, SEReturnType);
    }
  }
}

struct Type* Expr(struct Node* node) {
  // expression -> var ASSIGN expression | simple_expression
  #ifdef SEMANTIC_DEBUG
  printf("Expr(%d)\n", node->lineno);
  #endif
  node = node->child;
  struct Type* type = NULL;
  if(!strncmp(node->name, "Var", 3)) {
    type = Assign(node);
  } else if(!strncmp(node->name, "SimpleExpr", 10)) {
    type = SimpleExpr(node);
  }
  return type;
}

struct Type* Assign(struct Node* node) {
  // assign -> var ASSIGN expression
  #ifdef SEMANTIC_DEBUG
  printf("Assign(%d)\n", node->lineno);
  #endif
  struct Type* varType = Var(node);
  struct Type* exprType = Expr(node->next_sib);
  if(!isTypeMatch(varType, exprType)) {
    g_SEType1 = varType;
    g_SEType2 = exprType;
    SemanticError(node->lineno, node->column, SEAssignType);
  }
  return varType;
}

struct Type* Var(struct Node* node) {
  // var -> ID | ID [ expression ]
  #ifdef SEMANTIC_DEBUG
  printf("Var(%d)\n", node->lineno);
  #endif
  node = node->child;
  struct SymNode* varSym = lookup(node->str_term);
  if(!varSym) {
    SemanticError(node->lineno, node->column, SEUsedBeforeDecl);
    return NULL;
  }
  struct Type* varType = varSym->symType;

  if(node->next_sib) {
    // ID [ expression ]
    if(varType->typeKind != ArrayK) {
      g_SENode1 = node;
      SemanticError(node->lineno, node->column, SENotArray);
    }

    struct Type* exprType = Expr(node->next_sib);
    if(exprType->typeKind != BasicK || exprType->basic != Int) {
      SemanticError(node->lineno, node->column, SEInvalidIndex);
    }
  }

  return varType;
}

struct Type* SimpleExpr(struct Node* node) {
  // simple_expression -> additive_expression
  //                    | additive_expression relop additive_expression
  #ifdef SEMANTIC_DEBUG
  printf("SimpleExpr(%d)\n", node->lineno);
  #endif
  node = node->child;
  struct Type* addExprType1 = AdditiveExpr(node);
  if(node->next_sib) {
    // additive_expression relop additive_expression
    struct Type* addExprType2 = AdditiveExpr(node->next_sib->next_sib);
    if(!isTypeMatch(addExprType1, addExprType2)) {
      g_SEType1 = addExprType1;
      g_SEType2 = addExprType2;
      g_SENode1 = node->next_sib;
      SemanticError(node->lineno, node->column, SEOpTypeNotMatch);
    }
  }
  return addExprType1;
}

struct Type* AdditiveExpr(struct Node* node) {
  // additive_expression -> term | additive_expression addop term
  #ifdef SEMANTIC_DEBUG
  printf("AdditiveExpr(%d)\n", node->lineno);
  #endif
  node = node->child;
  struct Type* type1 = NULL;
  if(!strncmp(node->name, "Term", 4)) {
    type1 = Term(node);
  } else if(node->next_sib) {
    // additive_expression addop term
    type1 = AdditiveExpr(node);
    struct Type* type2 = Term(node->next_sib->next_sib);
    if(!isTypeMatch(type1, type2)) {
      g_SEType1 = type1;
      g_SEType2 = type2;
      g_SENode1 = node->next_sib;
      SemanticError(node->lineno, node->column, SEOpTypeNotMatch);
    }
  }
  return type1;
}

struct Type* Term(struct Node* node) {
  // term -> term mulop factor | factor
  #ifdef SEMANTIC_DEBUG
  printf("Term(%d)\n", node->lineno);
  #endif
  node = node->child;
  struct Type* type1 = NULL;
  if(!strncmp(node->name, "Factor", 6)) {
    type1 = Factor(node);
  } else if(node->next_sib) {
    type1 = Term(node);
    struct Type* type2 = Factor(node->next_sib->next_sib);
    if(!isTypeMatch(type1, type2)) {
      g_SEType1 = type1;
      g_SEType2 = type2;
      g_SENode1 = node->next_sib;
      SemanticError(node->lineno, node->column, SEOpTypeNotMatch);
    }
  }
  return type1;
}

struct Type* Factor(struct Node* node) {
  // factor -> ( expression ) | var | call | NUM
  #ifdef SEMANTIC_DEBUG
  printf("Factor(%d)\n", node->lineno);
  #endif
  node = node->child;
  struct Type* type = NULL;

  if(!strncmp(node->name, "Expr", 4)) {
    type = Expr(node);
  } else if(!strncmp(node->name, "Var", 3)) {
    type = Var(node);
  } else if(!strncmp(node->name, "Call", 4)) {
    type = Call(node);
  } else if(!strncmp(node->name, "NUM", 3)) {
    if(node->termKind == TermKNum) {
      type = createType(BasicK);
      if(node->is_int) {
        setBasic(type, Int);
      } else {
        setBasic(type, Real);
      }
    }
  }
  return type;
}

struct Type* Call(struct Node* node) {
  // call -> ID ( args )
  #ifdef SEMANTIC_DEBUG
  printf("Call(%d)\n", node->lineno);
  #endif
  node = node->child;
  struct SymNode* IDSym = lookup(node->str_term);
  
  if(!IDSym) {
    SemanticError(node->lineno, node->column, SEUsedBeforeDecl);
    return NULL;
  } else {
    struct Type* funcType = IDSym->symType;
    struct FuncArgList* argsType = Args(node->next_sib);
    if(!isFuncArgListMatch(funcType->func->nextArg, argsType)) {
      g_SENode1 = node;
      SemanticError(node->lineno, node->column, SEFuncArgsNotMatch);
    }

    return funcType->func->argType;
  }
}

struct FuncArgList* Args(struct Node* node) {
  // args -> arg_list | e
  #ifdef SEMANTIC_DEBUG
  printf("Args(%d)\n", node->lineno);
  #endif
  struct FuncArgList* argList;
  if(node->child) {
    argList = ArgList(node->child);
  } else {
    argList = createFuncArg("");
    argList->argType = createType(BasicK);
    setBasic(argList->argType, Void);
  }
  return argList;
}

struct FuncArgList* ArgList(struct Node* node) {
  // arg_list -> expression | arg_list, expression
  #ifdef SEMANTIC_DEBUG
  printf("ArgList(%d)\n", node->lineno);
  #endif
  node = node->child;
  struct FuncArgList* head, *tail;
  head = tail = NULL;
  struct Node* p = node;
  while(p) {
    struct Type* exprType = Expr(p);
    struct FuncArgList* tmp = createFuncArg("");
    tmp->argType = exprType;
    if(!head && !tail) {
      head = tail = tmp;
    } else {
      tail->nextArg = tmp;
      tail = tmp;
    }

    p = p->next_sib;
  }

  return head;
}
