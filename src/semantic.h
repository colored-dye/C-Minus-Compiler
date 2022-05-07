#ifndef _SEMANTIC_H_
#define _SEMANTIC_H_

#include "symtable.h"
#include "node.h"

// extern const char* g_BuiltinFunction[2];
typedef enum {
  SEReturnType, SEConditionNotNum, SEAssignType, SEUsedBeforeDecl,
  SEInvalidIndex, SENotArray, SEOpTypeNotMatch, SEFuncArgsNotMatch,
  SEFuncReturnArray,
} SemanticErrorKind;

void SemanticError(int lineno, int column, SemanticErrorKind errK);

void SemanticAnalysis(struct Node* root);
void Program(struct Node* node);
void DeclList(struct Node* node);
void Decl(struct Node* node);
void VarDecl(struct Node* node);
struct Type* TypeSpec(struct Node* node);
void FuncDecl(struct Node* node);
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
