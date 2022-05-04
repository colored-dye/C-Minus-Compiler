#include <stdio.h>
#include "node.h"
#include "y.tab.h"

extern int yydebug;
extern struct Node* g_root;

int main() {
  // yydebug = 1;
  yyparse();
  printTree(g_root);
  return 0;
}
