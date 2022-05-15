#include "semantic.h"
#include "node.hpp"
#include "symtable.h"
#include <stdio.h>
#include <string.h>

struct SymTable* g_SymTableStackTop = NULL;
struct SymTable* g_SymTableStackBottom = NULL;

struct Type g_FuncReturnType;

struct Node *g_SENode1, *g_SENode2;
struct Type *g_SEType1, *g_SEType2;

int g_firstPassError = 0;
int g_secondPassError = 0;

void SemanticError(int lineno, int column, SemanticErrorKind errK) {
  fprintf(stderr, "Error at line %d: ", lineno);
  const int speclen = 256;
  char spec[speclen];
  const char *type1, *type2;

  switch(errK) {
  case SEReturnType:
    type1 = Type2Str(g_SEType1);
    type2 = Type2Str(g_SEType2);
    snprintf(spec, speclen, "Return type `%s' function's return type `%s'.", type1, type2);
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
  case SEFuncReturnArray:
    snprintf(spec, speclen, "Array cannot be return value.");
    break;
  case SERedefinition:
    snprintf(spec, speclen, "Redefinition of `%s'.", g_SENode1->str_term);
    break;
  case SENotFunction:
    snprintf(spec, speclen, "Variable `%s' used as function.", g_SENode1->str_term);
    break;
  case SEInvalidLVal:
    snprintf(spec, speclen, "Invalid left value: `%s'\n", g_SENode1->str_term);
    break;
  case SEInvalidArraySize:
    snprintf(spec, speclen, "`%s' is not a valid array size.", g_SENode1->name);
    break;
  // default:
  //   break;
  }
  fprintf(stderr, "%s\n", spec);
  #ifdef SEMANTIC_DEBUG
  puts("Symbol table:");
  printSymTable(g_SymTableStackTop);
  #endif
}

int SemanticAnalysis(struct Node* root) {
  #ifdef SEMANTIC_DEBUG
  puts("Semantic Analysis Started\nFirst Pass Started");
  #endif

  Program(root->child, 1);
  #ifdef SEMANTIC_DEBUG
  puts("First Pass Finished");
  #endif
  if(!g_firstPassError) {
    #ifdef SEMANTIC_DEBUG
    puts("---------------------\nSecond Pass Started");
    #endif
    Program(root->child, 0);
    #ifdef SEMANTIC_DEBUG
    puts("Second Pass Finished");
    #endif
  } else {
    #ifdef SEMANTIC_DEBUG
    puts("No Second Pass~");
    #endif
  }

  #ifdef SEMANTIC_DEBUG
  puts("Semantic Analysis Finished");
  #endif

  return g_firstPassError || g_secondPassError;
}

void Program(struct Node* node, int first_pass) {
  #ifdef SEMANTIC_DEBUG
  printf("Program(%d)\n", node->lineno);
  #endif

  if(first_pass) {
    // Build global symbol table
    struct SymTable* globalTable = createSymTable();
    pushSymTable(globalTable);

    // Prepare builtin functions; input(), output()
    // int input(void)
    struct SymNode* builtin = createSymNode("input");
    builtin->symType = createType(FuncK);
    builtin->symType->func = createFuncArg("");
    builtin->symType->func->argType = createType(BasicK);
    setBasic(builtin->symType->func->argType, Int); // Return: int
    builtin->symType->func->nextArg = createFuncArg("");
    builtin->symType->func->nextArg->argType = createType(BasicK);
    setBasic(builtin->symType->func->nextArg->argType, Void); // Arg: void
    insert(globalTable, builtin);

    // void output(int)
    builtin = createSymNode("output");
    builtin->symType = createType(FuncK);
    builtin->symType->func = createFuncArg("");
    builtin->symType->func->argType = createType(BasicK);
    setBasic(builtin->symType->func->argType, Void); // Return: void
    builtin->symType->func->nextArg = createFuncArg("");
    builtin->symType->func->nextArg->argType = createType(BasicK);
    setBasic(builtin->symType->func->nextArg->argType, Int); // Arg: Int
    insert(globalTable, builtin);
  }
  
  DeclList(node->child, first_pass);
}

void DeclList(struct Node* node, int first_pass) {
  // declaration_list -> declaration | declaration_list declaration
  #ifdef SEMANTIC_DEBUG
  printf("DeclList(%d)\n", node->lineno);
  #endif
  struct Node* p = node;
  while(p) {
    Decl(p, first_pass);
    p = p->next_sib;
  }
}

void Decl(struct Node* node, int first_pass) {
  // declaration -> var_declaration | fun_declaration
  #ifdef SEMANTIC_DEBUG
  printf("Declaration(%d)\n", node->lineno);
  #endif
  node = node->child;
  if(!strncmp(node->name, "VarDecl", NAME_LENGTH)) {
    VarDecl(node, 1, first_pass);
  } else if(!strncmp(node->name, "FunDecl", NAME_LENGTH)) {
    FuncDecl(node, first_pass);
  } else {
    fprintf(stderr, "Fatal error in Decl(): Unrecognized node.\n");
  }
}

void VarDecl(struct Node* node, int global, int first_pass) {
  // var_declaration -> type_specifier id | type_specifier id [NUM]
  #ifdef SEMANTIC_DEBUG
  printf("VarDecl(%d)\n", node->lineno);
  #endif

  // 检查变量重定义
  if(global && first_pass) {
    // 第一遍扫描,只需要查看全局符号表
    if(lookup(node->child->next_sib->str_term)) {
      g_SENode1 = node->child->next_sib;
      SemanticError(node->lineno, node->column, SERedefinition);
      g_firstPassError = 1;
      return;
    }
  } else {
    // 第二遍扫描不用检查全局变量
    // 同一作用域内的变量不能重名
    if(!global) {
      if(lookupCurrent(node->child->next_sib->str_term)) {
        g_SENode1 = node->child->next_sib;
        SemanticError(node->lineno, node->column, SERedefinition);
        g_secondPassError = 1;
        return;
      }
    } else {
      return;
    }
  }

  struct Type* type = TypeSpec(node->child);
  if(node->child->next_sib->next_sib) {
    // 如果id后还有参数，就是array
    struct Type* array = createType(ArrayK);
    array->array.arrType = type;

    if(node->child->next_sib->next_sib->termKind != TermKNum || !node->child->next_sib->next_sib->is_int) {
      // 非法数组size
      g_SENode1 = node->child->next_sib->next_sib;
      SemanticError(node->lineno, node->column, SEInvalidArraySize);
      g_secondPassError = 1;
    }

    array->array.size = node->child->next_sib->next_sib->int_term;
    type = array;
  }
  struct SymNode* sym = createSymNode(node->child->next_sib->str_term);
  sym->symType = type;
  insert(g_SymTableStackTop, sym);
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

void FuncDecl(struct Node* node, int first_pass) {
  // fun_declaration -> type_specifier id ( params ) compound_stmt
  #ifdef SEMANTIC_DEBUG
  printf("FuncDecl(%d)\n", node->lineno);
  #endif
  char* id = node->child->next_sib->str_term;
  struct SymNode* sym = NULL;
  struct FuncArgList* params = NULL;
  struct Type* type, *rettype = NULL;

  if(first_pass) {
    if(lookupCurrent(node->child->next_sib->str_term)) {
      g_SENode1 = node->child->next_sib;
      SemanticError(node->lineno, node->column, SERedefinition);
      g_firstPassError = 1;
      return;
    }
    
    rettype = TypeSpec(node->child);
    if(rettype->typeKind == ArrayK) {
      SemanticError(node->lineno, node->column, SEFuncReturnArray);
      g_firstPassError = 1;
    }

    params = Params(node->child->next_sib->next_sib);
    rettype = TypeSpec(node->child);
    
    type = createType(FuncK);
    type->func = createFuncArg(id);
    type->func->argType = rettype;
    type->func->nextArg = params;
    
    sym = createSymNode(id);
    sym->symType = type;
    // 函数定义是全局的,对应符号表栈底
    insert(g_SymTableStackBottom, sym);
  } else {
    sym = lookup(id);
    rettype = sym->symType->func->argType;
    params = sym->symType->func->nextArg;
    
    // 函数的参数也要加入函数本地变量的符号表
    // 不需要创建符号表的情况: 参数为void; 没有定义局部变量
    int has_symTable = 0;
    if((params->argType->typeKind != BasicK || params->argType->basic != Void) 
        || (node->child->next_sib->next_sib->next_sib->child->child != NULL))
    {
      has_symTable = 1;

      struct SymTable* argTable = createSymTable();
      pushSymTable(argTable);

      struct FuncArgList* arg = params;
      while(arg) {
        if(arg->argType->typeKind != BasicK || arg->argType->basic != Void) {
          sym = createSymNode(arg->argName);
          sym->symType = arg->argType;
          insert(argTable, sym);
        }
        arg = arg->nextArg;
      }
    }

    // Store return type for ReturnStmt() to check
    g_FuncReturnType = *rettype;

    // Check compound_stmt
    CompoundStmt(node->child->next_sib->next_sib->next_sib);
    if(has_symTable) {
      // 删除函数的符号表
      popSymTable();
    }
  }
}

struct FuncArgList* Params(struct Node* node) {
  // params -> param_list | void
  #ifdef SEMANTIC_DEBUG
  printf("Params(%d)\n", node->lineno);
  #endif
  node = node->child;
  if(node->is_terminal && node->termKind == TermKType && !strncmp(node->str_term, "VOID", 4)) {
    struct FuncArgList* arg = createFuncArg("");
    arg->argType = createType(BasicK);
    setBasic(arg->argType, Void);
    return arg;
  } else {
    return ParamList(node);
  }
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
  
  node = node->child;
  struct FuncArgList* arg = NULL;
  struct Type* type = TypeSpec(node);
  arg = createFuncArg(node->next_sib->str_term);
  if(node->next_sib->next_sib) {
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
    VarDecl(p, 0, 0);
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
    fprintf(stderr, "Fatal error in Stmt(): Empty statement!\n");
    return;
  }
  int has_symbolTable = 0;
  switch(node->name[0]) {
  case 'E': // ExprStmt
    ExprStmt(node); break;
  case 'C': // CompoundStmt
    if(node->child->child != NULL) {
      // Push to symbol table stack
      pushSymTable(createSymTable());
      has_symbolTable = 1;
    }
    CompoundStmt(node);
    if(has_symbolTable) {
      // Pop symbol table stack
      popSymTable();
    }
    break;
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
  if(exprType->typeKind != BasicK || exprType->basic == Void) {
    SemanticError(node->child->lineno, node->child->column, SEConditionNotNum);
    g_secondPassError = 1;
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
  if(exprType->typeKind != BasicK || exprType->basic == Void) {
    SemanticError(node->child->lineno, node->child->column, SEConditionNotNum);
    g_secondPassError = 1;
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
      g_secondPassError = 1;
    }
  } else {
    struct Type* retType = Expr(node->child);
    int match = 1;
    if(!isTypeMatch(retType, &g_FuncReturnType)) {
      g_SEType2 = &g_FuncReturnType;
      g_SEType1 = retType;
      SemanticError(node->lineno, node->column, SEReturnType);
      g_secondPassError = 1;
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
  node = node->child;
  struct Type* varType = Var(node);
  struct Type* exprType = Expr(node->next_sib);

  // 非法左值
  // if(varType->) {

  // }

  // 左值和右值的类型不匹配
  if(!isTypeMatch(varType, exprType)) {
    g_SEType1 = varType;
    g_SEType2 = exprType;
    SemanticError(node->lineno, node->column, SEAssignType);
    g_secondPassError = 1;
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
    g_SENode1 = node;
    SemanticError(node->lineno, node->column, SEUsedBeforeDecl);
    g_secondPassError = 1;
    return NULL;
  }
  struct Type* varType = varSym->symType;

  if(node->next_sib) {
    // ID [ expression ]
    if(varType->typeKind != ArrayK) {
      g_SENode1 = node;
      SemanticError(node->lineno, node->column, SENotArray);
      g_secondPassError = 1;
    }

    struct Type* exprType = Expr(node->next_sib);
    if(exprType->typeKind != BasicK || exprType->basic != Int) {
      SemanticError(node->lineno, node->column, SEInvalidIndex);
      g_secondPassError = 1;
    }
    
    varType = varType->array.arrType;
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
      g_secondPassError = 1;
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
      g_secondPassError = 1;
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
      g_secondPassError = 1;
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
  } else if(node->termKind == TermKNum) {
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
    g_secondPassError = 1;
    return NULL;
  } else {
    if(IDSym->symType->typeKind != FuncK) {
      g_SENode1 = node;
      SemanticError(node->lineno, node->column, SENotFunction);
      g_secondPassError = 1;
      return IDSym->symType;
    } else {
      struct Type* funcType = IDSym->symType;
      struct FuncArgList* argsList = Args(node->next_sib);
      if(!isFuncArgListMatch(funcType->func->nextArg, argsList)) {
        g_SENode1 = node;
        SemanticError(node->lineno, node->column, SEFuncArgsNotMatch);
        g_secondPassError = 1;
      }

      return funcType->func->argType;
    }
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
