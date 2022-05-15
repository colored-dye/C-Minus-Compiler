/*
 * @Author: colored-dye
 * @Date: 2022-05-08 14:51:44
 * @LastEditors: SiO-2
 * @LastEditTime: 2022-05-15 19:10:44
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

ASTNode *ParserTreeToAST(struct Node *parserNode);
#endif