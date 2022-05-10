/*
 * @Author: SiO-2
 * @Date: 2022-05-09 10:31:35
 * @LastEditors: SiO-2
 * @LastEditTime: 2022-05-10 23:58:11
 * @FilePath: /C-Minus-Compiler/src/ast.hpp
 * @Description: Convert the parse tree to AST for subsequent LLVM operations.
 *
 * Copyright (c) 2022 by SiO-2, All Rights Reserved.
 */

#ifndef AST_H_
#define AST_H_

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
 * @brief Base class for all expression nodes. Lineno and column are initialized to (-1,-1).
 */
class ASTNode
{
    int lineno, column;

public:
    // Initialized to (-1,-1).
    ASTNode() : lineno(-1), column(-1) {}
    virtual ~ASTNode() {}

    void SetLineno(int setLineno) { lineno = setLineno; }
    int GetLineno() { return lineno; }
    void SetColumn(int setColumn) { column = setColumn; }
    int GetColumn() { return column; }
};

/**
 * @brief A program consists of a list (or sequence) of declarations,
 *  which may be function or variable declarations, in any order.
 */
class ProgramNode : public ASTNode
{
    vector<VarDecl *> VarDeclList;
    vector<FunDecl *> FunDeclList;

public:
    ProgramNode() {}
    void AddVarDecl(VarDecl *varDecl) { VarDeclList.push_back(varDecl); }
    void AddFunDecl(FunDecl *funDecl) { FunDeclList.push_back(funDecl); }
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
    vector<Param *> ParamList;

public:
    //
    FunDecl(TypeSpec typeSpec, string id) : typeSpec(typeSpec), id(id), haveParam(false) {}
    void AddParam(Param *param)
    {
        haveParam = true;
        ParamList.push_back(param);
    }
};

class Param : public ASTNode
{
    vector<int> test;

public:
    Param() {}
};

class CompoundStmt : public ASTNode
{
    vector<int> test;

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

class AddrExpr : public ASTNode
{
    vector<int> test;

public:
    AddrExpr() {}
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