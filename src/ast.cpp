/*
 * @Author: SiO-2
 * @Date: 2022-05-09 10:31:29
 * @LastEditors: SiO-2
 * @LastEditTime: 2022-05-17 09:53:24
 * @FilePath: /C-Minus-Compiler/src/ast.cpp
 * @Description:
 *
 * Copyright (c) 2022 by SiO-2, All Rights Reserved.
 */

#include "ast.hpp"
#include <iostream>

using namespace std;

void PrintASTNode(const ASTNode *curNode)
{
    static int indent = -1;
    if (curNode == NULL)
        return;

    indent++;

    for (int i = 0; i < indent * 2; i++) // control indentation
        printf(" ");

    switch (curNode->GetNodeType())
    {
    case ASTNODE:
        break;
    case ASTPROGRAM:
    {
        ASTProgram *program = (ASTProgram *)curNode;
        printf("Program <line:%d>\n", program->GetLineno());
        vector<ASTNode *>::const_iterator decl = program->GetDeclList().begin();
        while (decl != program->GetDeclList().end())
        {
            PrintASTNode(*decl);
            decl++;
        }

        break;
    }
    case ASTVARDECL:
    {
        ASTVarDecl *varDecl = (ASTVarDecl *)curNode;
        string typeSpecStr = (varDecl->GetTypeSpec() == ASTINT) ? "int" : "real";
        if (varDecl->IsArray())
            printf("VarDecl <line:%d> used %s '%s [%d]'\n", varDecl->GetLineno(), varDecl->GetId().c_str(), typeSpecStr.c_str(), varDecl->GetArrayLength());
        else
            printf("VarDecl <line:%d> used %s '%s'\n", varDecl->GetLineno(), varDecl->GetId().c_str(), typeSpecStr.c_str());

        break;
    }
    case ASTFUNDECL:
    {
        ASTFunDecl *funDecl = (ASTFunDecl *)curNode;
        string typeSpecStr = (funDecl->GetTypeSpec() == ASTINT)    ? "int"
                             : (funDecl->GetTypeSpec() == ASTREAL) ? "real"
                                                                   : "void";
        if (funDecl->HaveParam())
        {
            printf("FunDecl <line:%d> %s '%s'\n", funDecl->GetLineno(), funDecl->GetId().c_str(), typeSpecStr.c_str());
            vector<ASTParam *>::const_iterator param = funDecl->GetParamList().begin();
            while (param != funDecl->GetParamList().end())
            {
                PrintASTNode(*param);
                param++;
            }
        }
        else
            printf("FunDecl <line:%d> %s '%s (void)'\n", funDecl->GetLineno(), funDecl->GetId().c_str(), typeSpecStr.c_str());

        PrintASTNode(funDecl->GetCompoundStmt());

        break;
    }
    case ASTPARAM:
    {
        ASTParam *param = (ASTParam *)curNode;
        string typeSpecStr = (param->GetTypeSpec() == ASTINT)    ? "int"
                             : (param->GetTypeSpec() == ASTREAL) ? "real"
                                                                 : "void";
        if (param->IsArray())
            printf("Param <line:%d> %s '%s []'\n", param->GetLineno(), param->GetId().c_str(), typeSpecStr.c_str());
        else
            printf("Param <line:%d> %s '%s'\n", param->GetLineno(), param->GetId().c_str(), typeSpecStr.c_str());

        break;
    }
    case ASTCOMPOUNDSTMT:
    {
        ASTCompoundStmt *compoundStmt = (ASTCompoundStmt *)curNode;
        printf("CompoundStmt <line:%d>\n", compoundStmt->GetLineno());
        vector<ASTNode *>::const_iterator declOrStmt = compoundStmt->GetDeclStmtList().begin();
        while (declOrStmt != compoundStmt->GetDeclStmtList().end())
        {
            PrintASTNode(*declOrStmt);
            declOrStmt++;
        }
        break;
    }
    case ASTEXPR:
    {
        ASTExpr *expr = (ASTExpr *)curNode;
        printf("Expr <line:%d>\n", expr->GetLineno());
        if (expr->IsAssignStmt())
        {
            PrintASTNode(expr->GetVar());
            PrintASTNode(expr->GetExpr());
        }
        else
            PrintASTNode(expr->GetSimpleExpr());

        break;
    }
    case ASTSELECTSTMT:
    {
        ASTSelectStmt *selectStmt = (ASTSelectStmt *)curNode;
        printf("SelectStmt <line:%d>\n", selectStmt->GetLineno());
        PrintASTNode(selectStmt->GetExpr());
        PrintASTNode(selectStmt->GetTrueStmt());
        PrintASTNode(selectStmt->GetFalseStmt());
        if (selectStmt->HaveElse())
            PrintASTNode(selectStmt->GetFalseStmt());

        break;
    }
    case ASTWHILESTMT:
    {
        ASTWhileStmt *whileStmt = (ASTWhileStmt *)curNode;
        printf("WhileStmt <line:%d>\n", whileStmt->GetLineno());
        PrintASTNode(whileStmt->GetExpr());
        PrintASTNode(whileStmt->GetStmt());

        break;
    }
    case ASTFORSTMT:
    {
        ASTForStmt *forStmt = (ASTForStmt *)curNode;
        printf("ForStmt <line:%d>\n", forStmt->GetLineno());
        if (forStmt->HaveForParam1())
        {
            PrintASTNode(forStmt->GetVar1());
            PrintASTNode(forStmt->GetExpr1());
        }
        PrintASTNode(forStmt->GetExpr2());
        if (forStmt->HaveForParam3())
        {
            PrintASTNode(forStmt->GetVar3());
            PrintASTNode(forStmt->GetExpr3());
        }
        PrintASTNode(forStmt->GetStmt());
        
        break;
    }
    case ASTRETURNSTMT:
    {
        break;
    }
    case ASTVAR:
    {
        break;
    }
    case ASTSIMPLEEXPR:
    {
        break;
    }
    case ASTADDEXPR:
    {
        break;
    }
    case ASTTERM:
    {
        break;
    }
    case ASTFACTOR:
    {
        break;
    }
    case ASTCALL:
    {
        break;
    }

    default:
        break;
    }
    indent--;
    return;
}

void PrintAST(const ASTProgram *program)
{
    if (program->GetNodeType() == ASTPROGRAM)
        PrintASTNode(program);
}