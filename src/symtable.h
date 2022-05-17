#ifndef _SYMTABLE_H_
#define _SYMTABLE_H_

/**
 * 单向链表形式的符号表
 */

#include "node.hpp"

extern struct SymTable* g_SymTableStackTop;
extern struct SymTable* g_SymTableStackBottom;

typedef enum { BasicK, ArrayK, FuncK, ErrorK } TypeKind;
typedef enum { Void, Int, Real } BasicType;

struct Type_ {
  TypeKind typeKind;
  union {
    BasicType basic;
    struct { struct Type_ *arrType; int size; } array;
    struct FuncArgList* func; // 函数: 第一个结点为返回值,后面的是参数
  };
};

struct Type_* createType(TypeKind typeK);
BasicType str2BasicType(const char* s);
void setBasic(struct Type_* type, BasicType basic);
int isTypeMatch(struct Type_* t1, struct Type_* t2);
char* Type2Str(struct Type_* type);

/***************************************************/

struct FuncArgList {
  char argName[NAME_LENGTH];
  struct Type_* argType;
  struct FuncArgList* nextArg;
};

struct FuncArgList* createFuncArg(const char* name);
int isFuncArgListMatch(struct FuncArgList* funcArgList1, struct FuncArgList* funcArgList2);
int isFuncArgMatch(struct FuncArgList* funcArg1, struct FuncArgList* funcArg2);

/***************************************************/

struct SymNode {
  struct Type_* symType;
  // char symName[NAME_LENGTH];
  char *symName;
  struct SymNode* nextSym;
};

struct SymNode* createSymNode(char* name);
struct SymNode* lookup(char *name);
struct SymNode* lookupCurrent(char* name);
struct SymNode* lookupSymTable(struct SymNode* symTable, char* name);

/***************************************************/

// 单链表实现符号表栈
struct SymTable {
  struct SymNode* dummyhead;
  struct SymTable* nextTable;
};

struct SymTable* createSymTable(void);
void addSymTable(struct SymTable* newTable);

void pushSymTable(struct SymTable* symTable);
struct SymTable* popSymTable();
void insert(struct SymTable* symTable, struct SymNode* node);
#ifdef SEMANTIC_DEBUG
void printSymTable(struct SymTable* symTable);
void printSymNode(struct SymNode* symNode);
void printType(struct Type_* type);
#endif

#endif
