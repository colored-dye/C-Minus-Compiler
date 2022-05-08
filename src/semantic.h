#ifndef _SEMANTIC_H_
#define _SEMANTIC_H_

#include "symtable.h"
#include "node.h"

typedef enum {
    SEReturnType // 返回值后表达式类型与函数定义不符
  , SEConditionNotNum // 条件表达式不是数值
  , SEAssignType // 赋值表达式左值和右值的类型不符
  , SEUsedBeforeDecl // 变量或函数使用时未定义
  , SEInvalidIndex // 数组下标处的表达式类型不是int
  , SENotArray // 将非数组类型当作数组进行寻址操作
  , SEOpTypeNotMatch // 运算符两侧表达式的类型不同
  , SEFuncArgsNotMatch // 函数调用时参数个数或类型错误
  , SEFuncReturnArray // 函数返回值为数组
  , SERedefinition // 变量或函数重定义,包括全局变量/函数重定义和局部变量重定义
  , SENotFunction // 变量被当作函数
  , SEInvalidLVal // 非法左值
  , SEInvalidArraySize // 数组定义时size非整型
  ,
} SemanticErrorKind;

void SemanticError(int lineno, int column, SemanticErrorKind errK);

int SemanticAnalysis(struct Node* root);

void Program(struct Node* node, int first_pass);
void DeclList(struct Node* node, int first_pass);
void Decl(struct Node* node, int first_pass);
void VarDecl(struct Node* node, int global, int first_pass);
struct Type* TypeSpec(struct Node* node);
void FuncDecl(struct Node* node, int first_pass);
struct FuncArgList* Params(struct Node* node);
struct FuncArgList* ParamList(struct Node* node);
struct FuncArgList* Param(struct Node* node);
void CompoundStmt(struct Node* node);
void LocalDecl(struct Node* node);
void StmtList(struct Node* node);
void Stmt(struct Node* node);
void ExprStmt(struct Node* node);
void SelectionStmt(struct Node* node);
void WhileStmt(struct Node* node);
void ForStmt(struct Node* node);
void ReturnStmt(struct Node* node);
struct Type* Expr(struct Node* node);
struct Type* Assign(struct Node* node);
struct Type* Var(struct Node* node);
struct Type* SimpleExpr(struct Node* node);
struct Type* AdditiveExpr(struct Node* node);
struct Type* Term(struct Node* node);
struct Type* Factor(struct Node* node);
struct Type* Call(struct Node* node);
struct FuncArgList* Args(struct Node* node);
struct FuncArgList* ArgList(struct Node* node);

#endif
