/*
 * @Author: SiO-2
 * @Date: 2022-05-08 14:51:44
 * @LastEditors: SiO-2
 * @LastEditTime: 2022-05-14 22:41:08
 * @FilePath: /C-Minus-Compiler/src/node.hpp
 * @Description:
 *
 * Copyright (c) 2022 by SiO-2, All Rights Reserved.
 */
#ifndef _NODE_H_
#define _NODE_H_

#include "y.tab.h"
#include "ast.hpp"

#include <stdlib.h>

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
    /*
    Program -> GlobalDeclList -> Decl -> VarDecl
                                      -> FunDecl
    */
    if (parserNode->name == NodeNames[0]) // Program 0
    {
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
    /*
    VarDecl -> TypeSpec -> Type
            -> ID
            -> NUM
    */
    else if (parserNode->name == NodeNames[3]) // VarDecl 3
    {
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
    /*
    FunDecl -> TypeSpec -> Type
            -> ID
            -> Params -> Params -> ParamList -> Param
                                -> Void
            -> CompoundStmt
    */
    else if (parserNode->name == NodeNames[4]) // FunDecl 4
    {
    }
    else if (parserNode->name == NodeNames[8]) // Param 8
    {
    }
    else if (parserNode->name == NodeNames[9]) // CompoundStmt 9
    {
    }
    else if (parserNode->name == NodeNames[14]) // Expr 14
    {
    }
    else if (parserNode->name == NodeNames[15]) // SelectStmt 15
    {
    }
    else if (parserNode->name == NodeNames[16]) // WhileStmt 16
    {
    }
    else if (parserNode->name == NodeNames[17]) // ForStmt 17
    {
    }
    else if (parserNode->name == NodeNames[21]) // ReturnStmt 21
    {
    }
    else if (parserNode->name == NodeNames[22]) // Var 22
    {
    }
    else if (parserNode->name == NodeNames[23]) // SimpleExpr 23
    {
    }
    else if (parserNode->name == NodeNames[24]) // AddExpr 24
    {
    }
    else if (parserNode->name == NodeNames[27]) // Term 27
    {
    }
    else if (parserNode->name == NodeNames[28]) // Factor 28
    {
    }
    else if (parserNode->name == NodeNames[30]) // Call 30
    {
    }
    else // other node
    {
    }

    return curASTNode;
}

#endif