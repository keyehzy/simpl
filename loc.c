#include "loc.h"

#include <stddef.h>
#include <string.h>
#include <stdlib.h>

location new_loc(const char *begin, const char* end) {
    return (location) {.begin = begin, .end = end};
}

char *as_str(location l) {
  size_t len  = (size_t)(l.end - l.begin);
  char * name = (char *)malloc(len + 1);
  strncpy(name, l.begin, len);
  name[len] = '\0';
  return name;
}
