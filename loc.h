#ifndef LOC_H
#define LOC_H

typedef struct {
  const char *begin;
  const char *end;
} location;

char* as_str(location l);

#endif
