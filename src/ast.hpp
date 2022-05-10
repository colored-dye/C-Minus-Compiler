/*
 * @Author: SiO-2
 * @Date: 2022-05-09 10:31:35
 * @LastEditors: SiO-2
 * @LastEditTime: 2022-05-10 22:06:10
 * @FilePath: /C-Minus-Compiler/src/ast.hpp
 * @Description: Convert the parse tree to AST for subsequent LLVM operations.
 *
 * Copyright (c) 2022 by SiO-2, All Rights Reserved.
 */

#ifndef AST_H_
#define AST_H_

#include "node.h"

#include <vector>

using namespace std;

extern const char *NodeNames[];

// ASTNode - Base class for all expression nodes.
class ASTNode
{
    int lineno, column;

public:
    ASTNode() {}
    virtual ~ASTNode() {}
    int SetLineno(int lineno) {}
};

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
    vector<int> test;

public:
    VarDecl() {}
};

class FunDecl : public ASTNode
{
    vector<int> test;

public:
    FunDecl() {}
};

class Params : public ASTNode
{
    vector<int> test;

public:
    Params() {}
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