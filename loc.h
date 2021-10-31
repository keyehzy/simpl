#ifndef LOC_H
#define LOC_H

typedef struct {
  const char *begin;
  const char *end;
} location;

location new_loc(const char *, const char*);
char* as_str(location l);

#endif
