#ifndef _SYMTABLE_H_
#define _SYMTABLE_H_

/**
 * 单向链表形式的符号表
 */

#include "node.h"

typedef enum {
  SEReturnType, SEConditionNotNum, SEAssignType, SEUsedBeforeDecl,
  SEInvalidIndex, SENotArray, SEOpTypeNotMatch, SEFuncArgsNotMatch
} SemanticErrorKind;

typedef enum { BasicK, ArrayK, FuncK, ErrorK } TypeKind;
typedef enum { Void, Int, Real } BasicType;

struct Type {
  TypeKind typeKind;
  union {
    BasicType basic;
    struct { struct Type *arrType; int size; } array;
    struct FuncArgList* func; // 函数: 第一个结点为返回值,后面的是参数u
  };
};

struct Type* createType(TypeKind typeK);
void setBasic(struct Type* type, BasicType basic);
int isTypeMatch(struct Type* t1, struct Type* t2);
char* Type2Str(struct Type* type);

/***************************************************/

struct FuncArgList {
  char argName[NAME_LENGTH];
  struct Type* argType;
  struct FuncArgList* nextArg;
};

struct FuncArgList* createFuncArg(const char* name);
int isFuncArgListMatch(struct FuncArgList* funcArgList1, struct FuncArgList* funcArgList2);
int isFuncArgMatch(struct FuncArgList* funcArg1, struct FuncArgList* funcArg2);

/***************************************************/

struct SymNode {
  struct Type* symType;
  // char symName[NAME_LENGTH];
  char *symName;
  struct SymNode* nextSym;
};

struct SymNode* createSymNode(char* name);
struct SymNode* lookup(char *name);
void insert(struct SymNode* node);

BasicType str2BasicType(const char* s);

#endif
