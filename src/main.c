/*
 * @Author: colored-dye
 * @Date: 2022-05-08 14:51:44
 * @LastEditors: SiO-2
 * @LastEditTime: 2022-05-09 13:10:43
 * @FilePath: /C-Minus-Compiler/src/main.c
 * @Description:
 *
 * Copyright (c) 2022 by colored-dye, All Rights Reserved.
 */

#include <stdio.h>
#include "node.h"
#include "semantic.h"
#include "y.tab.h"

extern int yydebug;
extern FILE *yyin;
extern struct Node *g_root;
extern int g_syntaxError;

int main(int argc, char *argv[])
{
  // yydebug = 1; // 输出shift/reduce的全过程
  if (argc == 2)
  {
    FILE *input = fopen(argv[1], "rb");
    yyin = input;
    if (yyparse())
    {
      printf("Parse failure\n");
    }
    fclose(input);
  }
  else
  {
    if (yyparse())
    {
      printf("Parse failure\n");
    }
  }

  if (g_syntaxError)
  {
    printf("Found %d error%s in Syntax Analysis.\n", g_syntaxError, g_syntaxError > 1 ? "s" : "");
  }
  else
  {
    puts("No error found in Syntax Analysis~");
  }

  if (!SemanticAnalysis(g_root))
  {
    puts("No error found in Semantic Analysis~");
  }
  else
  {
    puts("Error found in Semantic Analysis");
  }
  
  // 文字形式输出语法树
  printTree(g_root);

  return 0;
}
