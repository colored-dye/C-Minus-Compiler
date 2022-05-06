#include <stdio.h>
#include "node.h"
#include "semantic.h"
#include "y.tab.h"

extern int yydebug;
extern FILE* yyin;
extern struct Node* g_root;

int main(int argc, char* argv[]) {
  // yydebug = 1; // 输出shift/reduce的全过程
  if(argc == 2) {
    FILE* input = fopen(argv[1], "rb");
    yyin = input;
    yyparse();
    fclose(input);
  } else {
    yyparse();
  }
  
  // printTree(g_root);
  SemanticAnalysis(g_root);
  return 0;
}
