/*
 * @Author: colored-dye
 * @Date: 2022-05-08 14:51:44
 * @LastEditors: SiO-2
 * @LastEditTime: 2022-05-15 00:38:44
 * @FilePath: /C-Minus-Compiler/src/node.hpp
 * @Description:
 *
 * Copyright (c) 2022 by colored-dye, All Rights Reserved.
 */
#ifndef _NODE_H_
#define _NODE_H_

#include "y.tab.h"
#include "ast.hpp"

#include <stdlib.h>
#include <string.h>
#include <strings.h>

#define NAME_LENGTH 32
#define STRING_LENGTH 128

typedef enum
{
    TermKType,
    TermKNum,
    TermKId,
    TermKOp,
    TermKBracket
} TermKind;

const char *NodeNames[] = {
    "Program",        // 0  entire program
    "GlobalDeclList", // 1  list of global declarations
    "Decl",           // 2  abstract declarations
    "VarDecl",        // 3  specific variable declaration
    "FunDecl",        // 4  specific function declaration
    "TypeSpec",       // 5  type specification
    "Params",         // 6
    "ParamList",      // 7
    "Param",          // 8
    "CompoundStmt",   // 9
    "LocalDecl",      // 10
    "StmtList",       // 11
    "Stmt",           // 12
    "ExprStmt",       // 13
    "Expr",           // 14
    "SelectionStmt",  // 15
    "WhileStmt",      // 16
    "ForStmt",        // 17
    "For_param1",     // 18
    "For_param2",     // 19
    "For_param3",     // 20
    "ReturnStmt",     // 21
    "Var",            // 22
    "SimpleExpr",     // 23
    "AddExpr",        // 24
    "RelOp",          // 25
    "AddOp",          // 26
    "Term",           // 27
    "Factor",         // 28
    "MulOp",          // 29
    "Call",           // 30
    "Args",           // 31
    "ArgList",        // 32
};

struct Node
{
    struct Node *child;     //第一个孩子;
    struct Node *next_sib;  //兄弟节点
    char name[NAME_LENGTH]; //节点名称
    // int nodeK;

    // Non-terminal只需关注其名称或类别
    // 而terminal需要支持所有的原生数据类型
    int is_terminal;
    TermKind termKind;
    union
    {
        struct
        {
            union
            {
                int int_term; // integer operator
                float real_term;
            };
            int is_int;
        };
        char str_term[STRING_LENGTH]; // id type
    };

    int column; //列数;
    int lineno;
};

struct Node *createNode(void);

struct Node *makeNode(const char *name);

void addChild(int n, struct Node *parent, ...);

void printTree(struct Node *root);

ASTNode *ParserTreeToAST(struct Node *parserNode)
{
    if (parserNode == NULL)
        return;

    ASTNode *curASTNode = NULL;
    if (parserNode->name == NodeNames[0]) // Program 0
    {
        /*
        Program -> GlobalDeclList -> Decl -> VarDecl
                                          -> FunDecl
        */
        curASTNode = new Program();
        curASTNode->SetASTNodeData(parserNode->lineno, parserNode->column, ASTPROGRAM);
        struct Node *decl = parserNode->child->child; // 跳过非 ASTNode 的 GlobalDeclList
        while (decl != NULL)
        {
            if (decl->child->name == NodeNames[3]) // VarDecl 3
            {
                ASTNode *varDecl = ParserTreeToAST(decl->child);
                ((Program *)curASTNode)->AddDecl(varDecl);
            }
            else if (decl->child->name == NodeNames[4]) // FunDecl 4
            {
                ASTNode *funDecl = ParserTreeToAST(decl->child);
                ((Program *)curASTNode)->AddDecl(funDecl);
            }
            decl = decl->next_sib;
        }
    }
    else if (parserNode->name == NodeNames[3]) // VarDecl 3
    {
        /*
        VarDecl -> TypeSpec -> Type
                -> ID
                -> NUM
        */
        ASTTypeSpec typeSpec = (parserNode->child->child->str_term == "INT") ? ASTINT : ASTREAL; // VarDecl->TypeSpec->Type
        string id = string(parserNode->child->next_sib->str_term);
        if (parserNode->child->next_sib->next_sib == NULL)
            curASTNode = new VarDecl(typeSpec, id);
        else
        {
            int arrayLength = parserNode->child->next_sib->next_sib->int_term;
            curASTNode = new VarDecl(typeSpec, id, arrayLength);
        }
    }
    else if (parserNode->name == NodeNames[4]) // FunDecl 4
    {
        /*
        FunDecl -> TypeSpec -> Type
                -> ID
                -> Params -> ParamList -> Param
                          -> Void
                -> CompoundStmt
        */
    }
    else if (parserNode->name == NodeNames[8]) // Param 8
    {
        /*
        Param -> TypeSpec -> Type
              -> ID
              -> [] ?
        */
    }
    else if (parserNode->name == NodeNames[9]) // CompoundStmt 9
    {
        /*
        CompoundStmt -> LocalDecl -> VarDecl
                     -> StmtList -> Stmt - CompoundStmt
                                         - Expr
                                         - SelectStmt
                                         - WhileStmt
                                         - ForStmt
                                         - ReturnStmt
        */
    }
    else if (parserNode->name == NodeNames[14]) // Expr 14
    {
        /*
        Expr -> SimpleExpr
             -> Assign -> Var
                       -> Expr
        */
    }
    else if (parserNode->name == NodeNames[15]) // SelectStmt 15
    {
        /*
        SelectStmt -> Expr
                   -> Stmt - CompoundStmt
                           - Expr
                           - SelectStmt
                           - WhileStmt
                           - ForStmt
                           - ReturnStmt
                   -> Stmt
        */
    }
    else if (parserNode->name == NodeNames[16]) // WhileStmt 16
    {
        /*
        WhileStmt -> Expr
                  -> Stmt - CompoundStmt
                          - Expr
                          - SelectStmt
                          - WhileStmt
                          - ForStmt
                          - ReturnStmt
        */
    }
    else if (parserNode->name == NodeNames[17]) // ForStmt 17
    {
        /*
        ForStmt -> For_param1 -> Var
                            -> Expr
                -> For_param2 -> Expr
                -> For_param3 -> Var
                            -> Expr
                -> Stmt - CompoundStmt
                        - Expr
                        - SelectStmt
                        - WhileStmt
                        - ForStmt
                        - ReturnStmt
        */
    }
    else if (parserNode->name == NodeNames[21]) // ReturnStmt 21
    {
        /*
        ReturnStmt -> Expr
        */
    }
    else if (parserNode->name == NodeNames[22]) // Var 22
    {
        /*
        Var -> ID
            -> Expr
        */
    }
    else if (parserNode->name == NodeNames[23]) // SimpleExpr 23
    {
        /*
        SimpleExpr -> AddExpr
                   -> RelOp
                   -> AddExpr
        */
    }
    else if (parserNode->name == NodeNames[24]) // AddExpr 24
    {
        /*
        AddExpr -> AddExpr
                -> AddOp
                -> Term
        */
    }
    else if (parserNode->name == NodeNames[27]) // Term 27
    {
        /*
        Term -> Term
             -> MulOp
             -> Factor
        */
        struct Node *p = parserNode->child;
        vector<Node *> stack;
        while (p && strcasecmp(p->name, "Term"))
        {
            stack.push_back(p);
            p = p->child;
        }
        curASTNode = new Term((Factor *)ParserTreeToAST(p->child));
        stack.pop_back();
        while (!stack.empty())
        {
            p = stack.back();
            stack.pop_back();
            ASTMulOp mulop;
            if (p->next_sib->int_term == MUL)
            {
                mulop = ASTMUL;
            }
            else
            {
                mulop = ASTDIV;
            }
            ((Term *)curASTNode)->AddTerm(mulop, (Factor *)ParserTreeToAST(p->next_sib->next_sib));
        }
    }
    else if (parserNode->name == NodeNames[28]) // Factor 28
    {
        /*
        Factor - Expr
               - Var
               - Call
               - NUM
        */
        if (!strncasecmp(parserNode->child->name, "Expr", 4))
        {
            curASTNode = new Factor((Expr *)(ParserTreeToAST(parserNode->child)));
        }
        else if (!strncasecmp(parserNode->child->name, "Var", 3))
        {
            curASTNode = new Factor((Var *)ParserTreeToAST(parserNode->child));
        }
        else if (!strncasecmp(parserNode->child->name, "Call", 4))
        {
            curASTNode = new Factor((Call *)ParserTreeToAST(parserNode->child));
        }
        else if (parserNode->child->termKind == TermKNum)
        {
            if (parserNode->child->is_int)
            {
                curASTNode = new Factor(parserNode->child->int_term);
            }
            else
            {
                curASTNode = new Factor(parserNode->child->real_term);
            }
        }
    }
    else if (parserNode->name == NodeNames[30]) // Call 30
    {
        /*
        Call -> ID
             -> Args -> ArgList -> Expr
        */
        curASTNode = new Call(parserNode->child->str_term);
        struct Node *p = parserNode->child->next_sib;
        while (p)
        {
            ((Call *)curASTNode)->AddArg((Expr *)ParserTreeToAST(p));
            p = p->next_sib;
        }
    }
    else // other node
    {
    }

    return curASTNode;
}

#endif