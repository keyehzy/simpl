#include "match.h"

bool compare_operand(const Node *pattern, const Node *target) {
  bool compare_kind = (pattern->wildcard.kind == operand_any) ||
    (pattern->wildcard.kind == target->operand.kind);
  bool compare_hash = (pattern->hash == 0) || /* anonymous or */
    (pattern->hash == target->hash);
  return compare_kind && compare_hash;
}

bool compare_operator(const Node *pattern, const Node *target) {
  operation pattern_o = pattern->wildcard.operation;
  operation target_o = target->operator.operation;
  bool compare_op_kind = (pattern_o.kind == op_any) ||
    (pattern_o.kind == target_o.kind);
  bool compare_prec = (pattern_o.prec == prec_any) ||
    (pattern_o.prec == target_o.prec);
  bool compare_assoc = (pattern_o.assoc == assoc_any) ||
    (pattern_o.assoc == target_o.assoc);
  bool compare_hash = (pattern->hash == 0) || /* anonymous or */
    (pattern->hash == target->hash);
  return compare_op_kind && compare_prec && compare_assoc && compare_hash;
}

bool match_condition(const Node *pattern, const Node *target) {

  if((pattern->kind == Operator) && (target->kind == Operator)) {
    return compare_operator(pattern, target);
  }

  return false;  
}

bool match_pattern1(const Node *pattern, const Node *target) {
  if((pattern->kind == Operand) && (target->kind == Operand)) {
    return compare_operand(pattern, target);
  } else if(match_condition(pattern, target)) {
    bool match_left = match_pattern1(pattern->wildcard.left, target->operator.left);
    bool match_right = match_pattern1(pattern->wildcard.right, target->operator.right);
    return match_left && match_right;
  }     
  return false;
}

bool match_pattern(const Node *pattern, const Node *target) {

  if(match_pattern1(pattern, target)) {
    return true;
  }

  if(target->kind == Operator) {
    bool match_left = match_pattern(pattern, target->operator.left);
    bool match_right = match_pattern(pattern, target->operator.right);
    return (match_left || match_right);				    
  }
  return false;
}
