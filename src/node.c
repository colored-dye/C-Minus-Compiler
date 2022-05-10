/*
 * @Author: colored-dye
 * @Date: 2022-05-08 14:51:44
 * @LastEditors: SiO-2
 * @LastEditTime: 2022-05-10 11:25:18
 * @FilePath: /C-Minus-Compiler/src/node.c
 * @Description:
 *
 * Copyright (c) 2022 by colored-dye, All Rights Reserved.
 */

#include "node.h"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

const char *NodeNames[] = {
    "Program",        // entire program
    "GlobalDeclList", // list of global declarations
    "Decl",           // abstract declarations
    "VarDecl",        // specific variable declaration
    "FunDecl",        // specific function declaration
    "TypeSpec",       // type specification
    "Params",
    "ParamList",
    "Param",
    "CompountStmt",
    "LocalDecl",
    "StmtList",
    "Stmt",
    "ExprStmt",
    "Expr",
    "SelectionStmt",
    "WhileStmt",
    "ForStmt",
    "For_param1",
    "For_param2",
    "For_param3",
    "ReturnStmt",
    "Var",
    "SimpleExpr",
    "AddExpr",
    "RelOp",
    "AddOp",
    "Term",
    "Factor",
    "MulOp",
    "Call",
    "Args",
    "ArgList",
};

struct Node *createNode(void)
{
  struct Node *ret = (struct Node *)malloc(sizeof(struct Node));
  if (!ret)
  {
    fprintf(stderr, "createNode(): Out of memory\n");
    return NULL;
  }
  ret->child = ret->next_sib = NULL;
  ret->is_terminal = 0;
  return ret;
}

void printTree(struct Node *root)
{
  static int indent = 0;
  if (!root)
  {
    return;
  }
  for (int i = 0; i < indent << 1; i++)
    putchar(' ');

  if (root->is_terminal)
  {
    switch (root->termKind)
    {
    case TermKType:
      printf("  [Type: %s]\n", root->str_term);
      break;
    case TermKNum:
      printf("NUM");
      if (root->is_int)
        printf("  [Num: %d]\n", root->int_term);
      else
        printf("  [Num: %.3f]\n", root->real_term);
      break;
    case TermKId:
      printf("ID");
      printf("  [Id: %s]\n", root->str_term);
      break;
    case TermKOp:
      printf("  [Op: %d]\n", root->int_term);
      break;
    case TermKBracket:
      printf("  [Bracket: %s]\n", root->str_term);
      break;
    }
  }
  else
  {
    printf("%s (%d)\n", root->name, root->lineno);
  }
  indent++;
  printTree(root->child);
  indent--;
  printTree(root->next_sib);
}

void addChild(int n, struct Node *parent, ...)
{
  va_list arg_list;
  va_start(arg_list, parent);
  struct Node *p, *child;
  int i = 0;
  if (!parent->child)
  {
    child = va_arg(arg_list, struct Node *);
    parent->child = child;
    p = child;
    i = 1;
  }
  else
  {
    p = parent->child;
    while (p->next_sib)
    {
      p = p->next_sib;
    }
  }

  for (; i < n; i++)
  {
    child = va_arg(arg_list, struct Node *);
    p->next_sib = child;
    p = child;
  }
  va_end(arg_list);
}

struct Node *makeNode(const char *name)
{
  struct Node *ret = createNode();
  strncpy(ret->name, name, NAME_LENGTH);
  return ret;
}
