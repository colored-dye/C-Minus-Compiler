/*
 * @Author: SiO-2
 * @Date: 2022-05-09 10:31:29
 * @LastEditors: SiO-2
 * @LastEditTime: 2022-05-15 03:09:41
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
    Program program1;
    program1.SetCoordinate(1, 1);

    VarDecl varDecl1(ASTINT, "a");
    varDecl1.SetCoordinate(2, 1);
    program1.AddDecl(&varDecl1);

    FunDecl funDecl1(ASTINT, "main");
    funDecl1.SetCoordinate(4, 1);
    program1.AddDecl(&funDecl1);

    bool isArray = varDecl1.IsArray();
    bool haveParam = funDecl1.HaveParam();

    cout << "isArray = " << isArray << endl;
    cout << "haveParam = " << haveParam << endl;
    cout << "finished" << endl;
}