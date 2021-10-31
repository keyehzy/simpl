#include "loc.h"

#include <stddef.h>
#include <string.h>
#include <stdlib.h>

char *as_str(location l) {
  size_t len  = (size_t)(l.end - l.begin);
  char * name = (char *)malloc(len + 1);
  strncpy(name, l.begin, len);
  name[len] = '\0';
  return name;
}
