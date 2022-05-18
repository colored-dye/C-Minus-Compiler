/*
 * @Author: SiO-2
 * @Date: 2022-05-09 10:31:29
 * @LastEditors: SiO-2
 * @LastEditTime: 2022-05-17 16:38:24
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
        vector<ASTVarDecl *>::const_iterator decl = compoundStmt->GetDeclList().begin();
        while (decl != compoundStmt->GetDeclList().end())
        {
            PrintASTNode(*decl);
            decl++;
        }
        vector<ASTNode *>::const_iterator stmt = compoundStmt->GetStmtList().begin();
        while (stmt != compoundStmt->GetStmtList().end())
        {
            PrintASTNode(*stmt);
            stmt++;
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
        ASTReturnStmt *returnStmt = (ASTReturnStmt *)curNode;
        printf("ReturnStmt <line:%d>\n", returnStmt->GetLineno());
        if (!returnStmt->IsVoid())
            PrintASTNode(returnStmt->GetExpr());

        break;
    }
    case ASTVAR:
    {
        ASTVar *var = (ASTVar *)curNode;
        printf("Var <line:%d> '%s'", var->GetLineno(), var->GetId().c_str());
        if (var->HaveSubscript())
        {
            printf(" have subscript\n");
            PrintASTNode(var->GetSubscript());
        }
        else
            printf("\n");

        break;
    }
    case ASTSIMPLEEXPR:
    {
        ASTSimpleExpr *simpleExpr = (ASTSimpleExpr *)curNode;
        printf("SimpleExpr <line:%d>\n", simpleExpr->GetLineno());
        PrintASTNode(simpleExpr->GetLeftAddExpr());
        if (simpleExpr->HaveRightAddExpr())
        {
            string relOp;
            switch (simpleExpr->GetRelOp())
            {
            case ASTLE:
                relOp = "<=";
                break;
            case ASTLT:
                relOp = "<";
                break;
            case ASTGT:
                relOp = ">";
                break;
            case ASTGE:
                relOp = ">=";
                break;
            case ASTEQ:
                relOp = "==";
                break;
            case ASTNE:
                relOp = "!=";
                break;

            default:
                break;
            }

            for (int i = 0; i < (indent + 1) * 2; i++) // control indentation
                printf(" ");
            printf("RelOp '%s'\n", relOp.c_str());
            PrintASTNode(simpleExpr->GetRightAddExpr());
        }

        break;
    }
    case ASTADDEXPR:
    {
        ASTAddExpr *addExpr = (ASTAddExpr *)curNode;
        printf("AddExpr <line:%d>\n", addExpr->GetLineno());
        PrintASTNode(addExpr->GetFirstTerm());
        if (addExpr->AreMultipleTerms())
        {
            vector<ASTAddOp>::const_iterator addOp = addExpr->GetAddOpList().begin();
            vector<ASTTerm *>::const_iterator term = addExpr->GetTermList().begin();
            while (addOp != addExpr->GetAddOpList().end() && term != addExpr->GetTermList().end())
            {
                string addOpStr = ((*addOp) == ASTADD) ? "Add" : "Minus";
                for (int i = 0; i < (indent + 1) * 2; i++) // control indentation
                    printf(" ");
                printf("AddOp '%s'\n", addOpStr.c_str());
                PrintASTNode((*term));
                addOp++;
                term++;
            }
        }

        break;
    }
    case ASTTERM:
    {
        ASTTerm *term = (ASTTerm *)curNode;
        printf("Term <line:%d>\n", term->GetLineno());
        PrintASTNode(term->GetFirstFactor());
        if (term->AreMultipleFactors())
        {
            vector<ASTMulOp>::const_iterator mulOp = term->GetMulOpList().begin();
            vector<ASTFactor *>::const_iterator factor = term->GetFactorList().begin();
            while (mulOp != term->GetMulOpList().end() && factor != term->GetFactorList().end())
            {
                string mulOpStr = ((*mulOp) == ASTMUL) ? "Mul" : "Div";
                for (int i = 0; i < (indent + 1) * 2; i++) // control indentation
                    printf(" ");
                printf("MulOp '%s'\n", mulOpStr.c_str());
                PrintASTNode((*factor));
                mulOp++;
                factor++;
            }
        }

        break;
    }
    case ASTFACTOR:
    {
        ASTFactor *factor = (ASTFactor *)curNode;
        if (factor->IsInt())
            printf("Factor <line:%d> 'Int' %d\n", factor->GetLineno(), factor->GetNumInt());
        else if (factor->IsReal())
            printf("Factor <line:%d> 'Real' %f\n", factor->GetLineno(), factor->GetNumReal());
        else
        {
            printf("Factor <line:%d>\n", factor->GetLineno());
            if (factor->GetExpr())
                PrintASTNode(factor->GetExpr());
            else if (factor->GetVar())
                PrintASTNode(factor->GetVar());
            else if (factor->GetCallExpr())
                PrintASTNode(factor->GetCallExpr());
        }

        break;
    }
    case ASTCALL:
    {
        ASTCall *callExpr = (ASTCall *)curNode;
        printf("CallExpr <line:%d> '%s'\n", callExpr->GetLineno(), callExpr->GetId().c_str());
        vector<ASTExpr *>::const_iterator arg = callExpr->GetArgList().begin();
        while (arg != callExpr->GetArgList().end())
        {
            PrintASTNode((*arg));
            arg++;
        }
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