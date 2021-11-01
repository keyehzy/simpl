#ifndef MATCH_H
#define MATCH_H

#include "simpl.h"
#include <stdbool.h>

bool match_pattern(const Node *, const Node *);
const Node *canonize_tree(const Node *);

#endif
