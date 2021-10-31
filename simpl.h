#ifndef SIMPL_H
#define SIMPL_H

#include "assert.h"
#include "loc.h"
#include "lex.h"

enum Tag {
  Operand,
  Operator,
  Invalid,
};

typedef struct OperandNode {
  int hash;
} OperandNode;

typedef enum {
    op_none,
    op_binary_plus,
    op_binary_minus,
    op_binary_times,
    op_binary_div,
    op_unary_plus,
    op_unary_minus,
} op_kind;

typedef enum {
    prec_none,
    prec_addsub,
    prec_multdiv,
    prec_unary,
} precedence;

typedef enum {
    assoc_none,
    assoc_left,
    assoc_right,
} associativity;

typedef struct {
    op_kind       kind;
    precedence    prec;
    associativity assoc;
} operation;

typedef struct OperatorNode {
  operation operation;
  struct Node *left;
  struct Node *right;
} OperatorNode;

typedef struct Node {
  location loc;
  enum Tag kind;
  union {
    struct OperandNode operand;
    struct OperatorNode operator;
  };
} Node;

Node *new_operand(location);
Node *new_operator(location, operation, struct Node*, struct Node*);
Node *new_invalid(void);
Node *parse(lexer *);
Node *parse_head(lexer *);
Node *parse_rest(lexer *, Node*);

#endif
