#include "match.h"

#define EMPTY_HASH 5381

bool match_pattern1(const Node *pattern, const Node *target);

bool compare_operand(const Node *pattern, const Node *target) {
  bool compare_kind = (pattern->operand.kind == operand_any) ||
    (pattern->operand.kind == target->operand.kind);
  bool compare_hash = (pattern->operand.hash == EMPTY_HASH) || /* anonymous or */
    (pattern->operand.hash == target->operand.hash);
  return compare_kind && compare_hash;
}

bool compare_operator(const Node *pattern, const Node *target) {
  operation pattern_o = pattern->operator.operation;
  operation target_o = target->operator.operation;
  bool compare_op_kind = (pattern_o.kind == op_any) ||
    (pattern_o.kind == target_o.kind);
  bool compare_prec = (pattern_o.prec == prec_any) ||
    (pattern_o.prec == target_o.prec);
  bool compare_assoc = (pattern_o.assoc == assoc_any) ||
    (pattern_o.assoc == target_o.assoc);
  bool compare_hash = (pattern->hash == EMPTY_HASH) || /* anonymous or */
    (pattern->operator.hash == target->operator.hash);
  return compare_op_kind && compare_prec && compare_assoc && compare_hash;
}

bool match_children(const Node *pattern, const Node *target) {
  if ((LEFT(pattern)->kind == Operand && RIGHT(pattern)->kind == Operand) &&
      (LEFT(target)->kind == Operand && RIGHT(target)->kind == Operand)) {
    return compare_operand(LEFT(pattern), RIGHT(pattern)) == compare_operand(LEFT(target), RIGHT(target));
  }
  return false;
}

bool match_subtree(const Node *pattern, const Node *target) {
  if(compare_operator(pattern, target) && match_children(pattern, target))
    return true;

  bool match_left = match_pattern1(pattern->operator.left, target->operator.left);
  bool match_right = match_pattern1(pattern->operator.right, target->operator.right);
  return match_left && match_right;
}

bool match_pattern1(const Node *pattern, const Node *target) {
  if(pattern->kind == Any) {
    return true;
  } else if((pattern->kind == Operand) && (target->kind == Operand)) {
    return compare_operand(pattern, target);
  } else if((pattern->kind == Operator) && (target->kind == Operator)) {
    return match_subtree(pattern, target);
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
