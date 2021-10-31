#include "assert.h"

#include <stdio.h>

void __crash(const char* msg, const char* file, unsigned line,
             const char* func) {
  fprintf(stderr, "%s:%ud: assertion %s failed in function %s\n", file, line,
          msg, func);
  __builtin_trap();
}
