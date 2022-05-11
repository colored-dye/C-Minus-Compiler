/*
 * @Author: SiO-2
 * @Date: 2022-05-09 10:31:35
 * @LastEditors: SiO-2
 * @LastEditTime: 2022-05-11 11:44:32
 * @FilePath: /C-Minus-Compiler/src/ast.hpp
 * @Description: Convert the parse tree to AST for subsequent LLVM operations.
 *
 * Copyright (c) 2022 by SiO-2, All Rights Reserved.
 */

/**
 * TODO：
 *  1. 在使用 vector 记录子节点的 class 中，如何记录节点（语句块）的顺序？
 */

#ifndef CMinusCompiler_AST_H_
#define CMinusCompiler_AST_H_

#include "node.h"

#include <vector>
#include <string>
#include <variant>

using namespace std;

extern const char *NodeNames[];

/**
 * @brief Support three types void, int, real (ie float), respectively ASTVOID = 0, ASTINT = 1, ASTFLOAT = 2.
 */
enum TypeSpec
{
    ASTVOID = 0,
    ASTINT,
    ASTREAL
};

/**
 * @brief Number has two types, int and float.
 */
union ASTNUM
{
    int intNum;
    float floatNum;
};

/**
 * @brief ASTNODE, PROGRAM, VARDECL, FUNDECL, PARAM, COMPOUNDSTMT, LOCALDECL, EXPR, VAR,
 *  SIMPLEEXPR, ADDEXPR, TERM, FACTOR, CALL, SELECTSTMT, WHILESTMT, FORSTMT, RETURNSTMT.
 */
enum ASTNodeType
{
    ASTNODE = 0,
    PROGRAM,
    VARDECL,
    FUNDECL,
    PARAM,
    COMPOUNDSTMT,
    LOCALDECL,
    EXPR,
    VAR,
    SIMPLEEXPR,
    ADDEXPR,
    TERM,
    FACTOR,
    CALL,
    SELECTSTMT,
    WHILESTMT,
    FORSTMT,
    RETURNSTMT
};

/**
 * @brief Base class for all expression nodes. The target AST is a LC-RS binary tree.
 * @param {int} lineno - initialized to -1.
 * @param {int} colume - initialized to -1.
 * @param {ASTNodeType} astNodeType - initialized to ASTNODE.
 * @param {ASTNode*} leftChild - initialized to NULL.
 * @param {ASTNode*} rightSibling - initialized to NULL.
 */
class ASTNode
{
    int lineno, column;
    ASTNodeType nodeType;
    ASTNode *leftChild;
    ASTNode *rightSibling;

public:
    ASTNode()
    {
        lineno = -1;
        column = -1;
        nodeType = ASTNODE;
        leftChild = NULL;
        rightSibling = NULL;
    }
    virtual ~ASTNode() {}

    void SetLineno(int setLineno) { lineno = setLineno; }
    int GetLineno() { return lineno; }

    void SetColumn(int setColumn) { column = setColumn; }
    int GetColumn() { return column; }

    void SetNodeType(ASTNodeType setNodeType) { nodeType = setNodeType; }
    ASTNodeType GetNodeType() { return nodeType; }

    void SetLeftChild(ASTNode *setLeftChild) { leftChild = setLeftChild; }
    ASTNode *GetLeftChild() { return leftChild; }

    void SetRightSibling(ASTNode *setRightSibling) { rightSibling = setRightSibling; }
    ASTNode *GetRightSibling() { return rightSibling; }
};

/**
 * @brief A program consists of a list (or sequence) of declarations,
 *  which may be function or variable declarations, in any order.
 */
class ProgramNode : public ASTNode
{
public:
    ProgramNode() {}
};

class VarDecl : public ASTNode
{
    TypeSpec typeSpec;
    string id;
    ASTNUM num;

public:
    VarDecl(TypeSpec typeSpec, string id, int intNum) : typeSpec(typeSpec), id(id) { num.intNum = intNum; }
    VarDecl(TypeSpec typeSpec, string id, float floatNum) : typeSpec(typeSpec), id(id) { num.floatNum = floatNum; }
};

/**
 * @brief By default there are no parameters.
 */
class FunDecl : public ASTNode
{
    TypeSpec typeSpec;
    string id;
    bool haveParam; // Is there an param.
    vector<Param *> paramList;

public:
    FunDecl(TypeSpec typeSpec, string id, bool haveParam = false) : typeSpec(typeSpec), id(id), haveParam(haveParam) {}
    void AddParam(Param *param)
    {
        haveParam = true;
        paramList.push_back(param);
    }
};

/**
 * @brief By default is not an array.
 */
class Param : public ASTNode
{
    TypeSpec typeSpec;
    string id;
    bool isArray;

public:
    Param(TypeSpec typeSpec, string id, bool isArray = false) : typeSpec(typeSpec), id(id), isArray(isArray) {}
};

class CompoundStmt : public ASTNode
{
    vector<LocalDecl *> localDeclList;
    vector<Expr *> exprList;
    vector<SelectStmt *> selectStmtList;
    vector<WhileStmt *> whileStmtList;
    vector<ForStmt *> forStmtList;
    ReturnStmt *returnStmt;

public:
    CompoundStmt() {}
};

class LocalDecl : public ASTNode
{
    vector<int> test;

public:
    LocalDecl() {}
};

class Expr : public ASTNode
{
    vector<int> test;
    bool isAssignStmt; // Is it an assignment statement.

public:
    Expr() {}
};

class Var : public ASTNode
{
    vector<int> test;

public:
    Var() {}
};

class SimpleExpr : public ASTNode
{
    vector<int> test;
    bool isSingleAddrExpr; // Is it an single additive-expression.

public:
    SimpleExpr() {}
};

class AddExpr : public ASTNode
{
    vector<int> test;

public:
    AddExpr() {}
};

class Term : public ASTNode
{
    vector<int> test;

public:
    Term() {}
};

class Factor : public ASTNode
{
    vector<int> test;

public:
    Factor() {}
};

class Call : public ASTNode
{
    vector<int> test;

public:
    Call() {}
};

class SelectStmt : public ASTNode
{
    vector<int> test;
    bool haveElse; // Is there an else.

public:
    SelectStmt() {}
};

class WhileStmt : public ASTNode
{
    vector<int> test;

public:
    WhileStmt() {}
};

class ForStmt : public ASTNode
{
    vector<int> test;
    bool haveForParam1; // Is there an For_param1.
    bool haveForParam2; // Is there an For_param2.
    bool haveForParam3; // Is there an For_param3.

public:
    ForStmt() {}
};

class ReturnStmt : public ASTNode
{
    vector<int> test;
    bool haveExpr; // Is there an Expr.

public:
    ReturnStmt() {}
};

#endif