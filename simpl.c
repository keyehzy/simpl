#include "simpl.h"

#include <stdlib.h>

Node *new_operand(location loc, operand_kind kind) {
  OperandNode operand = {0};
  operand.kind = kind;

  Node *node = (Node*) malloc(sizeof(Node));
  *node = (Node) {0};
  node->loc = loc;
  node->kind = Operand;
  node->operand = operand;
  return node;
}

Node *new_operator(location loc, operation o, Node *left, Node *right) {
  OperatorNode operator = {0};
  operator.operation = o;
  operator.left = left;
  operator.right = right;
  
  Node *node = (Node*) malloc(sizeof(Node));
  *node = (Node) {0};
  node->loc = loc;
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

Node *parse_head(lexer *lex) {
  token next_token = l_peek(lex);
  switch(next_token.type) {
  case tk_identifier: {
    location operand_location = next_token.loc;
    l_skip(lex);
    return new_operand(operand_location, operand_variable_identifier);
  }
  case tk_number_literal: {
    location operand_location = next_token.loc;
    l_skip(lex);
    return new_operand(operand_location, operand_number_literal);
  }
  case tk_eof:
    return new_invalid();
  case tk_plus:
  case tk_minus:
  case tk_times:
  case tk_div:
  case tk_anonymous_identifier_pattern:
  case tk_anonymous_operator_pattern:
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
  case tk_anonymous_identifier_pattern:
  case tk_anonymous_operator_pattern:
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

    if(prec > o.prec ||
	   (prec == o.prec && o.assoc == assoc_right))
	  return head;

      l_skip(lex);
      Node *rest = parse1(lex, o.prec);
      ASSERT(rest->kind != Invalid);
      head = new_operator(new_loc(head->loc.begin, rest->loc.end), o, head, rest);
      break;
    }
    case tk_identifier:
    case tk_number_literal:
    case tk_eof:
    case tk_anonymous_identifier_pattern:
    case tk_anonymous_operator_pattern:
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
