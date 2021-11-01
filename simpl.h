#ifndef SIMPL_H
#define SIMPL_H

#include "assert.h"
#include "loc.h"
#include "lex.h"

typedef enum {
  Operand,
  Operator,
  Invalid,
} Tag;

typedef enum {
  operand_number_literal,
  operand_variable_identifier,
  operand_any,
} operand_kind;

typedef struct OperandNode {
  location loc;
  operand_kind kind;
} OperandNode;

typedef enum {
  op_none,
  op_binary_plus,
  op_binary_minus,
  op_binary_times,
  op_binary_div,
  op_unary_plus,
  op_unary_minus,
  op_any,
} op_kind;

typedef enum {
  prec_none,
  prec_addsub,
  prec_multdiv,
  prec_unary,
  prec_any,
} precedence;

typedef enum {
  assoc_none,
  assoc_left,
  assoc_right,
  assoc_any,
} associativity;

typedef struct {
  op_kind kind;
  precedence prec;
  associativity assoc;
} operation;

typedef struct OperatorNode {
  location loc;
  operation operation;
  struct Node *left;
  struct Node *right;
} OperatorNode;

typedef struct Node {
  location loc;
  Tag kind;
  unsigned long hash;
  union {
    struct OperandNode operand;
    struct OperatorNode operator;
  };
} Node;

Node *new_operand(location, operand_kind);
Node *new_operator(location, operation, struct Node*, struct Node*);
Node *new_invalid(void);
Node *parse(lexer *);
Node *parse1(lexer *lex, precedence prec);
Node *parse_head(lexer *);
Node *parse_rest(lexer *, Node*, precedence);

#endif
