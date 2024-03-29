#ifndef SIMPL_H
#define SIMPL_H

#include "assert.h"
#include "loc.h"
#include "lex.h"

#include <stdbool.h>

typedef enum {
  Operand,
  Operator,
  Any,
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
  unsigned long hash;
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
  unsigned long hash;
  struct Node *left;
  struct Node *right;
} OperatorNode;

typedef struct Node {
  location loc;
  Tag kind;
  unsigned long hash;
  unsigned int is_pattern;
  short index;
  union {
    struct OperandNode operand;
    struct OperatorNode operator;
  };
} Node;

#define LEFT(x) ((x)->operator.left)
#define RIGHT(x) ((x)->operator.right)

Node *new_operand(location, operand_kind);
Node *new_operator(location, operation, struct Node*, struct Node*);
Node *new_invalid(void);
Node *parse_expression(const char*);
Node *parse(lexer *);
Node *parse1(lexer *lex, precedence prec);
Node *parse_head(lexer *);
Node *parse_rest(lexer *, Node*, precedence);
unsigned long hash(const char *, const char *);
bool stricly_equal(const Node *, const Node *);

#endif
