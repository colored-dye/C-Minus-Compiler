#include "symtable.h"
#include "node.h"
#include <stddef.h>
#include <string.h>

struct SymNode* SymbolTableBegin = NULL;
struct SymNode* SymbolTableEnd = NULL;

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
  struct SymNode* p = SymbolTableBegin;
  while(p && strncmp(name, p->symName, NAME_LENGTH)) {
    p = p->nextSym;
  }
  return p;
}

void insert(struct SymNode* node) {
  if(SymbolTableBegin == NULL && SymbolTableEnd == NULL) {
    SymbolTableBegin = SymbolTableEnd = node;
  } else {
    SymbolTableEnd->nextSym = node;
    SymbolTableEnd = node;
  }
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
  // if(strncmp(funcArg1->argName, funcArg2->argName, NAME_LENGTH)) {
  //   return 0;
  // }
  // 只需要比较类型
  if(!isTypeMatch(funcArg1->argType, funcArg2->argType)) {
    return 0;
  }

  return 1;
}

int isTypeMatch(struct Type* t1, struct Type* t2) {
  int match = 1;

  if(t1->typeKind == t2->typeKind) {
    switch(t1->typeKind) {
    case BasicK:
      if(t1->basic != t2->basic) {
        match = 0;
      }
      break;
    case ArrayK:
      if(!isTypeMatch(t1->array.arrType, t2->array.arrType) || t1->array.size != t2->array.size) {
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
  char* ret = NULL;
  switch (type->typeKind) {
  case BasicK:
    switch(type->basic) {
    case Void: ret = "void"; break;
    case Int: ret = "int"; break;
    case Real: ret = "real"; break;
    }
  case ArrayK:
    // switch(type->array.arrType) {
    // case 
    // }
    ret = "array"; break;
  case FuncK:
    ret = "function"; break;
  case ErrorK:
    ret = "Error"; break;
    break;
  }
  return ret;
}
