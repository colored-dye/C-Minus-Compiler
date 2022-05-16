/*
 * @Author: SiO-2
 * @Date: 2022-05-09 10:31:29
 * @LastEditors: SiO-2
 * @LastEditTime: 2022-05-16 23:21:27
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
    // control indentation
    for (int i = 0; i < indent * 2; i++)
        printf(" ");

    switch (curNode->GetNodeType())
    {
    case ASTNODE:
        break;
    case ASTPROGRAM:
    {
        ASTProgram *program = (ASTProgram *)curNode;
        printf("ASTProgram <line:%d>\n", program->GetLineno());
        vector<ASTNode *>::const_iterator decl = program->GetDeclList().begin();
        // indent++;
        while (decl != program->GetDeclList().end())
        {
            PrintASTNode(*decl);
            decl++;
        }
        // indent--;
        break;
    }
    case ASTVARDECL:
    {
        ASTVarDecl *varDecl = (ASTVarDecl *)curNode;
        string typeSpecStr = (varDecl->GetTypeSpec() == ASTINT) ? "int" : "real";
        if (varDecl->IsArray())
            printf("ASTVarDecl <line:%d> used %s '%s [%d]'\n", varDecl->GetLineno(), varDecl->GetId().c_str(), typeSpecStr.c_str(), varDecl->GetArrayLength());
        else
            printf("ASTVarDecl <line:%d> used %s '%s'\n", varDecl->GetLineno(), varDecl->GetId().c_str(), typeSpecStr.c_str());
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
            printf("ASTFunDecl <line:%d> %s '%s'\n", funDecl->GetLineno(), funDecl->GetId().c_str(), typeSpecStr.c_str());
            vector<ASTParam *>::const_iterator param = funDecl->GetParamList().begin();
            // indent++;
            while (param != funDecl->GetParamList().end())
            {
                PrintASTNode(*param);
                param++;
            }
            // indent--;
        }
        else
        {
            printf("ASTFunDecl <line:%d> %s '%s (void)'\n", funDecl->GetLineno(), funDecl->GetId().c_str(), typeSpecStr.c_str());
        }
        // indent++;
        PrintASTNode(funDecl->GetCompoundStmt());
        // indent--;

        break;
    }
    case ASTPARAM:
    {
        ASTParam *param = (ASTParam *)curNode;
        string typeSpecStr = (param->GetTypeSpec() == ASTINT)    ? "int"
                             : (param->GetTypeSpec() == ASTREAL) ? "real"
                                                                 : "void";
        if (param->IsArray())
            printf("ASTParam <line:%d> %s '%s []'\n", param->GetLineno(), param->GetId().c_str(), typeSpecStr.c_str());
        else
            printf("ASTParam <line:%d> %s '%s'\n", param->GetLineno(), param->GetId().c_str(), typeSpecStr.c_str());
        break;
    }
    case ASTCOMPOUNDSTMT:
    {
        break;
    }
    case ASTEXPR:
    {
        break;
    }
    case ASTSELECTSTMT:
    {
        break;
    }
    case ASTWHILESTMT:
    {
        break;
    }
    case ASTFORSTMT:
    {
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
    PrintASTNode(program);
}