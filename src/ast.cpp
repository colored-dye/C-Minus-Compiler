/*
 * @Author: SiO-2
 * @Date: 2022-05-09 10:31:29
 * @LastEditors: SiO-2
 * @LastEditTime: 2022-05-16 21:10:21
 * @FilePath: /C-Minus-Compiler/src/ast.cpp
 * @Description:
 *
 * Copyright (c) 2022 by SiO-2, All Rights Reserved.
 */

#include "ast.hpp"

#include <iostream>
using namespace std;

void PrintASTNode(ASTNode *curNode)
{
    static int indent = 0;

    if (curNode == NULL)
        return;

    for (int i = 0; i < indent; i++)
        printf(" ");

    switch (curNode->GetNodeType())
    {
    case ASTNODE:
        break;
    case ASTPROGRAM:
        Program *program = (Program *)curNode;
        vector<ASTNode *>::const_iterator decl;
        for (decl = program->GetDeclList().begin(); decl != program->GetDeclList().end(); decl++)
        {
            PrintASTNode(decl);
        }
        break;
    case ASTVARDECL:
        break;
    case ASTFUNDECL:
        break;
    case ASTPARAM:
        break;
    case ASTCOMPOUNDSTMT:
        break;
    case ASTEXPR:
        break;
    case ASTSELECTSTMT:
        break;
    case ASTWHILESTMT:
        break;
    case ASTFORSTMT:
        break;
    case ASTRETURNSTMT:
        break;
    case ASTVAR:
        break;
    case ASTSIMPLEEXPR:
        break;
    case ASTADDEXPR:
        break;
    case ASTTERM:
        break;
    case ASTFACTOR:
        break;
    case ASTCALL:
        break;

    default:
        break;
    }
    return;
}