#ifndef SIMPL_H
#define SIMPL_H

#include "assert.h"
#include "loc.h"

enum Tag {
  Operand,
  Operator,
};

struct OperandNode {
  int children_size;
};


/* TODO: make precedence work before we write the parser */
struct OperatorNode {
  int children_size;
  struct Node *left;
  struct Node *right;
};

struct Node {
  location loc;
  enum Tag kind;
  union {
    struct OperandNode operand;
    struct OperatorNode operator;
  };
};
#endif
  
  
  
