/*
 * @Author: SiO-2
 * @Date: 2022-05-09 10:31:29
 * @LastEditors: SiO-2
 * @LastEditTime: 2022-05-17 10:06:32
 * @FilePath: /C-Minus-Compiler/src/asttest.cpp
 * @Description:
 *
 * Copyright (c) 2022 by SiO-2, All Rights Reserved.
 */

#include "ast.hpp"

#include <iostream>
using namespace std;

int main()
{
    ASTProgram program1;
    program1.SetCoordinate(1, 1);

    ASTVarDecl varDecl1(ASTINT, "a", 10);
    varDecl1.SetCoordinate(2, 1);
    program1.AddDecl(&varDecl1);

    ASTFunDecl funDecl1(ASTINT, "main");
    funDecl1.SetCoordinate(4, 1);
    program1.AddDecl(&funDecl1);

    ASTParam param1(ASTINT, "b", true);
    param1.SetCoordinate(4, 1);
    funDecl1.AddParam(&param1);

    ASTCompoundStmt compoundStmt1;
    compoundStmt1.SetCoordinate(5, 1);
    funDecl1.AddCompoundStmt(&compoundStmt1);

    // bool isArray = varDecl1.IsArray();
    // bool haveParam = funDecl1.HaveParam();
    // cout << varDecl1.GetId() << " isArray = " << isArray << endl;
    // cout << funDecl1.GetId() << " haveParam = " << haveParam << endl;
    // cout << "finished" << endl;

    PrintASTNode(&program1);
}