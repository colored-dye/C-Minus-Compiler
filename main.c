#include <stdio.h>
#include "global.h"

extern int yydebug;

int main() {
  yydebug = 1;
  while (!yylex()) {
    
  }
  return 0;
}
