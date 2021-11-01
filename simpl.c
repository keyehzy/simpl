#include "simpl.h"

#include <stdlib.h>

unsigned long hash(const char *begin, const char *end) {
  unsigned long hash = 5381;
  int c;

  for(const char *it = begin; begin != end; begin++) {
    c = it[0];
    hash = ((hash << 5) + hash) + (unsigned long) c;
  }

  return hash;
}

Node *new_operand(location loc, operand_kind kind) {
  OperandNode operand = {0};
  operand.loc = loc;
  operand.kind = kind;

  Node *node = (Node*) malloc(sizeof(Node));
  *node = (Node) {0};
  node->loc = loc;
  node->hash = hash(loc.begin, loc.end);
  node->kind = Operand;
  node->operand = operand;
  return node;
}

Node *new_operator(location loc, operation o, Node *left, Node *right) {
  OperatorNode operator = {0};
  operator.loc = loc;
  operator.operation = o;
  operator.left = left;
  operator.right = right;
  
  Node *node = (Node*) malloc(sizeof(Node));
  *node = (Node) {0};
  node->loc = new_loc(left->loc.begin, right->loc.end);
  node->hash = hash(loc.begin, loc.end);
  node->kind = Operator;
  node->operator = operator;
  
  return node;  
}

Node *new_invalid() {
  Node *node = (Node*) malloc(sizeof(Node));
  *node = (Node) {0};
  node->kind = Invalid;
  return node;
}

struct PatternAttributes {
  Tag tag;
  operand_kind kind;
  operation operation;
};

struct PatternAttributes get_pattern_attr(Tag context, token pattern) {
  Tag tag = context;
  operand_kind kind = operand_any;
  operation o = (operation) {.kind = op_any, .prec = prec_any, .assoc = assoc_any};
  (void) pattern; /* TODO: Parse pattern to get attributes */
  return (struct PatternAttributes) {.tag = tag, .kind = kind, .operation = o};
}

Node *parse_head(lexer *lex) {
  token next_token = l_peek(lex);
  switch(next_token.type) {
  case tk_identifier: {
    location operand_location = next_token.loc;

    l_skip(lex);

    if(l_peek(lex).type == tk_identifier_pattern) {
      token pattern = l_peek(lex);
      struct PatternAttributes attr = get_pattern_attr(Operand, pattern);
      OperandNode operand = {0};
      operand.kind = attr.kind;
      Node *node = (Node*) malloc(sizeof(Node));
      *node = (Node) {0};
      node->loc = new_loc(operand_location.begin, pattern.loc.end);
      node->hash = hash(operand_location.begin, operand_location.end);
      node->kind = Operand;
      node->operand = operand;
      l_skip(lex);
      return node;
    }

    return new_operand(operand_location, operand_variable_identifier);
  }

  case tk_number_literal: {
    location operand_location = next_token.loc;
    l_skip(lex);
    return new_operand(operand_location, operand_number_literal);
  }

  case tk_identifier_pattern: {
    token pattern = next_token;
    struct PatternAttributes attr = get_pattern_attr(Operand, pattern);
    OperandNode operand = {0};
    operand.kind = attr.kind;
    Node *node = (Node*) malloc(sizeof(Node));
    *node = (Node) {0};
    node->loc = new_loc(pattern.loc.begin, pattern.loc.end);
    node->hash = 0;		/* match any */
    node->kind = Operand;
    node->operand = operand;
    l_skip(lex);
    return node;
  }

  case tk_eof:
    return new_invalid();
  case tk_plus:
  case tk_minus:
  case tk_times:
  case tk_div:
  case tk_operator_pattern:
  default:
    UNREACHABLE();
  }
  return NULL;
}

operation lookup_operation(token t) {
  switch(t.type) {
  case tk_plus:
    return (operation) {.kind = op_binary_plus, .prec = prec_addsub, .assoc = assoc_right};
  case tk_minus:
    return (operation) {.kind = op_binary_minus, .prec = prec_addsub, .assoc = assoc_right};
  case tk_times:
    return (operation) {.kind = op_binary_times, .prec = prec_multdiv, .assoc = assoc_right};
  case tk_div:
    return (operation) {.kind = op_binary_div, .prec = prec_multdiv, .assoc = assoc_right};
  case tk_identifier:
  case tk_number_literal:
  case tk_eof:
  case tk_identifier_pattern:
  case tk_operator_pattern:
  default:
    UNREACHABLE();
  }
  return (operation) {0};    
}

Node *parse_rest(lexer *lex, Node *head, precedence prec) {

  while(1) {
    token next_token = l_peek(lex);
    switch(next_token.type) {
    case tk_plus:
    case tk_minus:
    case tk_times:
    case tk_div: {
      operation o = lookup_operation(next_token);
      location op_loc = next_token.loc;

    if(prec > o.prec ||
	   (prec == o.prec && o.assoc == assoc_right))
	  return head;

      l_skip(lex);
      Node *rest = parse1(lex, o.prec);
      ASSERT(rest->kind != Invalid);
      head = new_operator(op_loc, o, head, rest);
      break;
    }
    case tk_identifier:
    case tk_number_literal:
    case tk_eof:
    case tk_identifier_pattern:
    case tk_operator_pattern:
    default:
      return head;
    }
  }
}

Node *parse1(lexer *lex, precedence prec) {
    Node *head = parse_head(lex);
    return parse_rest(lex, head, prec);
}

Node *parse(lexer *lex) {
  Node *head = parse_head(lex);
  return parse_rest(lex, head, (precedence) {0});
}
