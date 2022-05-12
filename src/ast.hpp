/*
 * @Author: SiO-2
 * @Date: 2022-05-09 10:31:35
 * @LastEditors: SiO-2
 * @LastEditTime: 2022-05-12 12:37:20
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
 * @brief ASTNODE, PROGRAM, VARDECL, FUNDECL, PARAM, COMPOUNDSTMT, EXPR, VAR, SIMPLEEXPR,
 *  ADDEXPR, TERM, FACTOR, CALL, SELECTSTMT, WHILESTMT, FORSTMT, RETURNSTMT.
 */
enum ASTNodeType
{
    ASTNODE = 0,
    PROGRAM,
    VARDECL,
    FUNDECL,
    PARAM,
    COMPOUNDSTMT,
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
 */
class ASTNode
{
    int lineno, column;
    ASTNodeType nodeType;
    // ASTNode *leftChild;
    // ASTNode *rightSibling;

public:
    ASTNode(int lineno = -1, int column = -1) : lineno(lineno), column(column)
    {
        nodeType = ASTNODE;
        // leftChild = NULL;
        // rightSibling = NULL;
    }
    virtual ~ASTNode() {}

    void SetLineno(int setLineno) { lineno = setLineno; }
    int GetLineno() { return lineno; }

    void SetColumn(int setColumn) { column = setColumn; }
    int GetColumn() { return column; }

    /**
     * @brief ASTNodeType - ASTNODE, PROGRAM, VARDECL, FUNDECL, PARAM, COMPOUNDSTMT, LOCALDECL, EXPR, VAR,
     *  SIMPLEEXPR, ADDEXPR, TERM, FACTOR, CALL, SELECTSTMT, WHILESTMT, FORSTMT, RETURNSTMT.
     */
    void SetNodeType(ASTNodeType setNodeType) { nodeType = setNodeType; }
    ASTNodeType GetNodeType() { return nodeType; }

    void SetASTNodeData(int lineno, int column, ASTNodeType nodeType)
    {
        ASTNode::lineno = lineno;
        ASTNode::column = column;
        ASTNode::nodeType = nodeType;
    }

    // void SetLeftChild(ASTNode *setLeftChild) { leftChild = setLeftChild; }
    // ASTNode *GetLeftChild() { return leftChild; }

    // void SetRightSibling(ASTNode *setRightSibling) { rightSibling = setRightSibling; }
    // ASTNode *GetRightSibling() { return rightSibling; }
};

/**
 * @brief A program consists of a list (or sequence) of declarations,
 *  which may be function or variable declarations, in any order.
 * @param {vector<ASTNode*>} Decl - There must be at least one declaration.
 *  The last declaration in a program must be a function declaration of the form void main(void).
 */
class Program : public ASTNode
{
    vector<ASTNode *> Decl;

public:
    Program() {}
};

/**
 * @brief A variable declaration declares either a simple variable of integer type or an array
 *  variable whose base type is integer or float, and whose indices range from 0 ... num-1.
 * @param {TypeSpec} typeSpec - int, real or void.
 * @param {string} id - identifier.
 * @param {bool} isArray - by default is not an array.
 * @param {int} arrayLength - array length.
 */
class VarDecl : public ASTNode
{
    TypeSpec typeSpec;
    string id;
    bool isArray;
    int arrayLength;

public:
    VarDecl(TypeSpec typeSpec, string id)
        : typeSpec(typeSpec), id(id), isArray(false) {}

    VarDecl(TypeSpec typeSpec, string id, int arrayLength)
        : typeSpec(typeSpec), id(id), isArray(true), arrayLength(arrayLength) {}
};

/**
 * @brief Function declaration.
 * @param {TypeSpec} typeSpec - return type specifier.
 * @param {string} id - identifier.
 * @param {bool} haveParam - flag of parameters. By default there are no parameters.
 * @param {vector<Param*>} paramList - comma-separated list of parameters inside parentheses.
 * @param {vector<ASTNode*>} compoundStmt - compound statement consists of curly brackets
 *  surrounding a set of declarations and statements.
 */
class FunDecl : public ASTNode
{
    TypeSpec typeSpec;
    string id;
    bool haveParam; // Is there an param.
    vector<Param *> paramList;
    vector<ASTNode *> compoundStmt;

public:
    FunDecl(TypeSpec typeSpec, string id, bool haveParam = false)
        : typeSpec(typeSpec), id(id), haveParam(haveParam) {}

    void AddParam(Param *param)
    {
        if (!haveParam)
            haveParam = true;
        paramList.push_back(param);
    }

    void AddStmt(ASTNode *stmt) { compoundStmt.push_back(stmt); };
};

/**
 * @brief Parameters of a function are either void (i.e., there  *  are no parameters) or
 *  a list representing the function's parameters. Parameters followed by brackets are array
 *  parameters whose size can vary. Simple integer parameters are passed by value. Array
 *  parameters are passed by reference (i.e., as pointers) and must be matched by an array
 *  variable during a call.
 * @param {TypeSpec} typeSpec - int, real or void.
 * @param {string} id - identifier.
 * @param {bool} isArray - by default is not an array.
 */
class Param : public ASTNode
{
    TypeSpec typeSpec;
    string id;
    bool isArray;

public:
    Param(TypeSpec typeSpec, string id, bool isArray = false)
        : typeSpec(typeSpec), id(id), isArray(isArray) {}
};

// class CompoundStmt : public ASTNode
// {
//     vector<LocalDecl *> localDeclList;
//     vector<Expr *> exprList;
//     vector<SelectStmt *> selectStmtList;
//     vector<WhileStmt *> whileStmtList;
//     vector<ForStmt *> forStmtList;
//     ReturnStmt *returnStmt;
//     vector<ASTNode *> localDeclAndStmtList;
// public:
//     CompoundStmt()
//     {
//     }
// };

class Expr : public ASTNode
{
    bool isAssignStmt; // Is it an assignment statement. By default it is not.
    Var *var;
    Expr *expr;

    SimpleExpr *simpleExpr;

public:
    Expr(SimpleExpr *simpleExpr)
        : isAssignStmt(false), var(NULL), expr(NULL), simpleExpr(simpleExpr) {}

    Expr(Var *var, Expr *expr)
        : isAssignStmt(true), var(var), expr(expr), simpleExpr(NULL) {}
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