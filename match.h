#ifndef MATCH_H
#define MATCH_H

#include "simpl.h"
#include <stdbool.h>

#define MATCHES_MAX_SZ 100

typedef struct {
  const Node *matches[MATCHES_MAX_SZ];
  int size;
} match_collector;

void collect_match(match_collector *, const Node *);
bool match_pattern(const Node *, const Node *);
match_collector match_and_collect_pattern(const Node *pattern, const Node *target);
const Node *canonize_tree(const Node *);

#endif
