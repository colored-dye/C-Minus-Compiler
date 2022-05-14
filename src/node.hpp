/*
 * @Author: colored-dye
 * @Date: 2022-05-08 14:51:44
 * @LastEditors: SiO-2
 * @LastEditTime: 2022-05-15 03:51:47
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
        return NULL;

    ASTNode *curASTNode = NULL;
    if (strncasecmp(parserNode->name, "Program", 7) == 0)
    {
        /*
        Program -> GlobalDeclList --> Decl - VarDecl
                                           - FunDecl
         */
        curASTNode = new Program();
        curASTNode->SetCoordinate(parserNode->lineno, parserNode->column);
        struct Node *decl = parserNode->child->child; // Skip GlobalDeclList that is not an ASTNode.
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
    else if (strncasecmp(parserNode->name, "VarDecl", 7) == 0)
    {
        /*
        VarDecl -> TypeSpec -> Type
                -> ID
                -- NUM
         */
        struct Node *pType = parserNode->child->child;
        struct Node *pId = parserNode->child->next_sib;
        struct Node *pNum = pId->next_sib;
        ASTTypeSpec typeSpec = (pType->str_term == "INT") ? ASTINT : ASTREAL;
        string id = string(pId->str_term);

        if (pNum == NULL)
            curASTNode = new VarDecl(typeSpec, id);
        else
        {
            int arrayLength = pNum->int_term;
            curASTNode = new VarDecl(typeSpec, id, arrayLength);
        }
        curASTNode->SetCoordinate(parserNode->lineno, parserNode->column);
    }
    else if (strncasecmp(parserNode->name, "FunDecl", 7) == 0)
    {
        /*
        FunDecl -> TypeSpec -> Type
                -> ID
                -> Params - ParamList --> Param
                          - Void
                -> CompoundStmt
         */
        struct Node *pType = parserNode->child->child;
        struct Node *pId = parserNode->child->next_sib;
        struct Node *pParams = pId->next_sib;
        struct Node *pCompoundStmt = pParams->next_sib;

        ASTTypeSpec typeSpec;
        if (pType->str_term == "INT")
            typeSpec = ASTINT;
        else if (pType->str_term == "REAL")
            typeSpec = ASTREAL;
        else if (pType->str_term == "VOID")
            typeSpec = ASTVOID;
        string id = string(pId->str_term);
        curASTNode = new FunDecl(typeSpec, id);
        curASTNode->SetCoordinate(parserNode->lineno, parserNode->column);

        if (pParams->child->str_term != "VOID")
        {
            struct Node *pParam = pParams->child->child; // Skip ParamList that is not an ASTNode.
            while (pParam != NULL)
            {
                ASTNode *param = ParserTreeToAST(pParam);
                ((FunDecl *)curASTNode)->AddParam((Param *)param);
                pParam = pParam->next_sib;
            }
        }

        CompoundStmt *compoundStmt = (CompoundStmt *)ParserTreeToAST(pCompoundStmt);
        ((FunDecl *)curASTNode)->AddCompoundStmt(compoundStmt);
    }
    else if (strncasecmp(parserNode->name, "Param", 5) == 0)
    {
        /*
        Param -> TypeSpec -> Type
              -> ID
              -- []
         */
        struct Node *pType = parserNode->child->child;
        struct Node *pId = parserNode->child->next_sib;
        struct Node *pBracket = pId->next_sib;

        ASTTypeSpec typeSpec;
        if (pType->str_term == "INT")
            typeSpec = ASTINT;
        else if (pType->str_term == "REAL")
            typeSpec = ASTREAL;
        else if (pType->str_term == "VOID")
            typeSpec = ASTVOID;
        string id = string(pId->str_term);
        if (pBracket == NULL)
            curASTNode = new Param(typeSpec, id);
        else
            curASTNode = new Param(typeSpec, id, true);

        curASTNode->SetCoordinate(parserNode->lineno, parserNode->column);
    }
    else if (strncasecmp(parserNode->name, "CompoundStmt", 12) == 0)
    {
        /*
        CompoundStmt -> LocalDecl --> VarDecl
                     -> StmtList --> Stmt - CompoundStmt
                                          - Expr
                                          - SelectStmt
                                          - WhileStmt
                                          - ForStmt
                                          - ReturnStmt
         */

        struct Node *pVarDecl = parserNode->child->child;
        struct Node *pStmt = parserNode->child->next_sib;
        curASTNode = new CompoundStmt();
        curASTNode->SetCoordinate(parserNode->lineno, parserNode->column);

        while (pVarDecl != NULL)
        {
            ASTNode *varDecl = ParserTreeToAST(pVarDecl);
            ((CompoundStmt *)curASTNode)->AddDecl(varDecl);
            pVarDecl = pVarDecl->next_sib;
        }
        while (pStmt != NULL)
        {
            ASTNode *stmt = ParserTreeToAST(pStmt->child);
            ((CompoundStmt *)curASTNode)->AddDecl(stmt);
            pStmt = pStmt->next_sib;
        }
    }
    else if (strncasecmp(parserNode->name, "Expr", 4) == 0)
    {
        /*
        Expr - SimpleExpr
             - Assign -> Var
                      -> Expr
         */
        if (strncasecmp(parserNode->child->name, "SimpleExpr", 10) == 0)
        {
            struct Node *pSimpleExpr = parserNode->child;
            ASTNode *simpleExpr = ParserTreeToAST(pSimpleExpr);
            curASTNode = new Expr((SimpleExpr *)simpleExpr);
        }
        else if (strncasecmp(parserNode->child->name, "Assign", 6) == 0)
        {
            struct Node *pVar = parserNode->child->child;
            ASTNode *var = ParserTreeToAST(pVar);
            struct Node *pExpr = pVar->next_sib;
            ASTNode *expr = ParserTreeToAST(pExpr);
            curASTNode = new Expr((Var *)var, (Expr *)expr);
        }

        curASTNode->SetCoordinate(parserNode->lineno, parserNode->column);
    }
    else if (strncasecmp(parserNode->name, "SelectionStmt", 13) == 0)
    {
        /*
        SelectStmt -> Expr
                   -> Stmt - CompoundStmt
                           - Expr
                           - SelectStmt
                           - WhileStmt
                           - ForStmt
                           - ReturnStmt
                   -- Stmt
         */
        struct Node *pExpr = parserNode->child;
        ASTNode *expr = ParserTreeToAST(pExpr);
        struct Node *pTrueStmt = pExpr->next_sib;
        ASTNode *trueStmt = ParserTreeToAST(pTrueStmt->child);
        struct Node *pFalseStmt = pTrueStmt->next_sib;
        if (pFalseStmt == NULL)
            curASTNode = new SelectStmt((Expr *)expr, (ASTNode *)trueStmt);
        else
        {
            ASTNode *falseStmt = ParserTreeToAST(pFalseStmt->child);
            curASTNode = new SelectStmt((Expr *)expr, (ASTNode *)trueStmt, (ASTNode *)falseStmt);
        }
        curASTNode->SetCoordinate(parserNode->lineno, parserNode->column);
    }
    else if (strncasecmp(parserNode->name, "WhileStmt", 9) == 0)
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
        struct Node *pExpr = parserNode->child;
        ASTNode *expr = ParserTreeToAST(pExpr);
        struct Node *pStmt = pExpr->next_sib;
        ASTNode *stmt = ParserTreeToAST(pStmt->child);
        curASTNode = new WhileStmt((Expr *)expr, (ASTNode *)stmt);
        curASTNode->SetCoordinate(parserNode->lineno, parserNode->column);
    }
    else if (strncasecmp(parserNode->name, "ForStmt", 7) == 0)
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
    else if (strncasecmp(parserNode->name, "ReturnStmt", 10) == 0)
    {
        /*
        ReturnStmt -- Expr
         */
        curASTNode = new ReturnStmt();
        curASTNode->SetCoordinate(parserNode->lineno, parserNode->column);
        struct Node *pExpr = parserNode->child;
        if (pExpr != NULL)
        {
            ASTNode *expr = ParserTreeToAST(pExpr);
            ((ReturnStmt *)curASTNode)->AddExpr((Expr *)expr);
        }
    }
    else if (strncasecmp(parserNode->name, "Var", 3) == 0)
    {
        /*
        Var -> ID
            -- Expr
         */
        struct Node *pId = parserNode->child;
        struct Node *pExpr = pId->next_sib;
        string id = string(pId->str_term);
        ASTNode *expr = ParserTreeToAST(pExpr);
        if (expr == NULL)
            curASTNode = new Var(id);
        else
        {
            ASTNode *expr = ParserTreeToAST(pExpr);
            curASTNode = new Var(id, (Expr *)expr);
        }
        curASTNode->SetCoordinate(parserNode->lineno, parserNode->column);
    }
    else if (strncasecmp(parserNode->name, "SimpleExpr", 10) == 0)
    {
        /*
        SimpleExpr -> AddExpr
                   -- RelOp
                   -- AddExpr
         */
        struct Node *pLeftAddExpr = parserNode->child;
        ASTNode *leftAddExpr = ParserTreeToAST(pLeftAddExpr);
        struct Node *pRelOp = pLeftAddExpr->next_sib;
        if (pRelOp == NULL)
            curASTNode = new SimpleExpr((AddExpr *)leftAddExpr);
        else
        {
            ASTRelOp relOp;
            switch (pRelOp->int_term)
            {
            case LE:
                relOp = ASTLE;
                break;
            case LT:
                relOp = ASTLT;
                break;
            case GT:
                relOp = ASTGT;
                break;
            case GE:
                relOp = ASTGE;
                break;
            case EQ:
                relOp = ASTEQ;
                break;
            case NE:
                relOp = ASTNE;
                break;

            default:
                break;
            }
            struct Node *pRightAddExpr = pRelOp->next_sib;
            ASTNode *rightAddExpr = ParserTreeToAST(pRightAddExpr);
            curASTNode = new SimpleExpr((AddExpr *)leftAddExpr, relOp, (AddExpr *)rightAddExpr);
        }
        curASTNode->SetCoordinate(parserNode->lineno, parserNode->column);
    }
    else if (strncasecmp(parserNode->name, "AddExpr", 7) == 0)
    {
        /*
        AddExpr -- AddExpr
                -- AddOp
                -> Term
         */
        if (strncasecmp(parserNode->child->name, "Term", 4) == 0)
        {
            struct Node *pTerm = parserNode->child;
            ASTNode *term = ParserTreeToAST(pTerm);
            curASTNode = new AddExpr((Term *)term);
        }
        else if (strncasecmp(parserNode->child->name, "AddExpr", 7) == 0)
        {
            struct Node *pAddExpr = parserNode->child;
            ASTNode *firstTerm = ParserTreeToAST(pAddExpr->child);
            curASTNode = new AddExpr((Term *)firstTerm);

            struct Node *pAddOp = pAddExpr->next_sib;
            ASTAddOp addOp = (pAddOp->int_term == ADD) ? ASTADD : ASTMINUS;
            struct Node *pTerm = parserNode->child;
            ASTNode *term = ParserTreeToAST(pTerm);
            ((AddExpr *)curASTNode)->AddTerm(addOp, (Term *)term);
        }
        curASTNode->SetCoordinate(parserNode->lineno, parserNode->column);
    }
    else if (strncasecmp(parserNode->name, "Term", 4) == 0)
    {
        /*
        Term -- Term
             -- MulOp
             -> Factor
         */
        if (strncasecmp(parserNode->child->name, "Factor", 6) == 0)
        {
            struct Node *pFactor = parserNode->child;
            ASTNode *factor = ParserTreeToAST(pFactor);
            curASTNode = new Term((Factor *)factor);
        }
        else if (strncasecmp(parserNode->child->name, "Term", 4) == 0)
        {
            struct Node *pTerm = parserNode->child;
            ASTNode *firstFactor = ParserTreeToAST(pTerm->child);
            curASTNode = new Term((Factor *)firstFactor);

            struct Node *pMulOp = pTerm->next_sib;
            ASTMulOp addOp = (pMulOp->int_term == MUL) ? ASTMUL : ASTDIV;
            struct Node *pFactor = parserNode->child;
            ASTNode *factor = ParserTreeToAST(pFactor);
            ((Term *)curASTNode)->AddFactor(addOp, (Factor *)factor);
        }
        curASTNode->SetCoordinate(parserNode->lineno, parserNode->column);
    }
    else if (strncasecmp(parserNode->name, "Factor", 6) == 0)
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
    else if (strncasecmp(parserNode->name, "Call", 4) == 0)
    {
        /*
        Call -> ID
             -> Args -- ArgList --> Expr
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