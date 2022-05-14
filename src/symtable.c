#include "symtable.h"
#include "node.hpp"
#include <stdio.h>
#include <string.h>

// char g_TypeString[STRING_LENGTH];

BasicType str2BasicType(const char* s) {
  BasicType bt;
  if(!strncmp(s, "INT", 3)) {
    bt = Int;
  } else if(!strncmp(s, "REAL", 4)) {
    bt = Real;
  } else if(!strncmp(s, "VOID", 4)) {
    bt = Void;
  }
  return bt;
}

struct Type* createType(TypeKind typeK) {
  struct Type* ret = (struct Type*)malloc(sizeof(struct Type));
  ret->typeKind = typeK;
  return ret;
}

void setBasic(struct Type* type, BasicType basic) {
  memset(&type->basic, 0, sizeof(type->array));
  type->basic = basic;
}

struct FuncArgList* createFuncArg(const char* name) {
  struct FuncArgList* ret = (struct FuncArgList*)malloc(sizeof(struct FuncArgList));
  strncpy(ret->argName, name, NAME_LENGTH);
  return ret;
}

struct SymNode* createSymNode(char* name) {
  struct SymNode* ret = (struct SymNode*)malloc(sizeof(struct SymNode));
  // strncpy(ret->symName, name, NAME_LENGTH);
  ret->symName = name;
  ret->nextSym = NULL;
  ret->symType = NULL;
  return ret;
}

struct SymNode* lookup(char *name) {
  struct SymTable* tablep = g_SymTableStackTop;
  struct SymNode* symp = NULL;
  while(tablep) {
    symp = lookupSymTable(tablep->dummyhead->nextSym, name);
    if(symp)
      break;
    tablep = tablep->nextTable;
  }
  return symp;
}

struct SymNode* lookupCurrent(char* name) {
  return lookupSymTable(g_SymTableStackTop->dummyhead->nextSym, name);
}

struct SymNode* lookupSymTable(struct SymNode* symTable, char* name) {
  struct SymNode* nodep = symTable;
  while(nodep) {
    if(!strncmp(nodep->symName, name, NAME_LENGTH)) {
      break;
    }
    nodep = nodep->nextSym;
  }
  return nodep;
}

void insert(struct SymTable* symTable, struct SymNode* node) {
  // 头插法,在当前的符号表中插入新的符号
  node->nextSym = symTable->dummyhead->nextSym;
  symTable->dummyhead->nextSym = node;
}

int isFuncArgListMatch(struct FuncArgList* funcArgList1, struct FuncArgList* funcArgList2) {
  while(funcArgList1 && funcArgList2) {
    // 参数类型不匹配
    if(!isFuncArgMatch(funcArgList1, funcArgList2))
      return 0;
    funcArgList1 = funcArgList1->nextArg;
    funcArgList2 = funcArgList2->nextArg;
  }
  // 参数个数不相等
  if((!!funcArgList1) != (!!funcArgList2))
    return 0;
  return 1;
}
int isFuncArgMatch(struct FuncArgList* funcArg1, struct FuncArgList* funcArg2) {
  // 只需要比较参数类型,比较名称没有意义
  if(!isTypeMatch(funcArg1->argType, funcArg2->argType)) {
    return 0;
  }

  return 1;
}

int isTypeMatch(struct Type* t1, struct Type* t2) {
  int match = 1;

  if((!!t1) != (!!t2))
    return 0;

  if(t1->typeKind == t2->typeKind) {
    switch(t1->typeKind) {
    case BasicK:
      if(t1->basic != t2->basic) {
        match = 0;
      }
      break;
    case ArrayK:
      // 数组不应该比较大小
      if(!isTypeMatch(t1->array.arrType, t2->array.arrType)) {
        match = 0;
      }
      break;
    case FuncK:
      if(!isFuncArgListMatch(t1->func, t2->func)) {
        match = 0;
      }
      break;
    case ErrorK: break;
    default:
      break;
    }
  } else {
    match = 0;
  }

  return match;
}

char* Type2Str(struct Type* type) {
  char* ret = (char*)malloc(STRING_LENGTH), *tmp = NULL;
  struct FuncArgList* argp = NULL;
  switch (type->typeKind) {
  case BasicK:
    switch(type->basic) {
    case Void: sprintf(ret, "void"); break;
    case Int: sprintf(ret, "int"); break;
    case Real: sprintf(ret, "real"); break;
    }
    break;
  case ArrayK:
    tmp = Type2Str(type->array.arrType);
    snprintf(ret, STRING_LENGTH, "array[%s]", tmp);
    free(tmp);
    break;
  case FuncK:
    snprintf(ret, STRING_LENGTH, "function(");
    argp = type->func->nextArg;
    while(argp) {
      strncat(ret, Type2Str(argp->argType), STRING_LENGTH);
      if(argp->nextArg)
        strncat(ret, ",", STRING_LENGTH);
      argp = argp->nextArg;
    }
    strncat(ret, ") -> ", STRING_LENGTH);
    strncat(ret, Type2Str(type->func->argType), STRING_LENGTH);
    break;
  case ErrorK:
    sprintf(ret, "Error"); break;
    break;
  }
  return ret;
}

struct SymTable* createSymTable(void) {
  struct SymTable* ret = (struct SymTable*)malloc(sizeof(struct SymTable));
  ret->dummyhead = createSymNode("");
  ret->nextTable = NULL;
  return ret;
}

void pushSymTable(struct SymTable* symTable) {
  if(!g_SymTableStackTop && !g_SymTableStackBottom) {
    g_SymTableStackTop = g_SymTableStackBottom = symTable;
  } else {
    // 头插法
    symTable->nextTable = g_SymTableStackTop;
    g_SymTableStackTop = symTable;
  }
}

struct SymTable* popSymTable() {
  // printSymTable(g_SymTableStackTop);
  struct SymTable* top = g_SymTableStackTop;
  if(!g_SymTableStackTop) {
    fprintf(stderr, "popSymTable(): SymTable stack empty!\n");
  }
  g_SymTableStackTop = top->nextTable;
  top->nextTable = NULL;
  if(!g_SymTableStackTop)
    g_SymTableStackBottom = NULL;
  return top;
}

#ifdef SEMANTIC_DEBUG
void printSymTable(struct SymTable* symTable) {
  puts("---------------------------------");
  while(symTable) {
    struct SymNode* nodep = symTable->dummyhead->nextSym;
    while(nodep) {
      printSymNode(nodep);
      nodep = nodep->nextSym;
    }
    symTable = symTable->nextTable;
    puts("---------------------------------");
  }
}

void printSymNode(struct SymNode* symNode) {
  printf("%s: ", symNode->symName);
  printType(symNode->symType);
}

void printType(struct Type* type) {
  printf("%s\n", Type2Str(type));
}

#endif
