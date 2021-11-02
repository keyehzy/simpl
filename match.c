#include "match.h"

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#define EMPTY_HASH 5381

bool compare_operand(const Node *pattern, const Node *target);

bool compare_operand_no_indexing(const Node *pattern, const Node *target);

bool match_pattern1(const Node *pattern, const Node *target);

#define OPERAND_TABLE_SZ 100
static const Node* operand_table[OPERAND_TABLE_SZ] = {0};
static int operand_table_index = 0;
static int tree_index = 0;

void canonize_tree1(Node *tree) {
  if(tree->kind == Operand) {

    for(int i = 0; i < operand_table_index; i++) {
      if(compare_operand_no_indexing(tree, operand_table[i])) {
	tree->index = operand_table[i]->index;
	return;	
      }
    }
    
    tree->index = tree_index++;
    operand_table[operand_table_index++] = tree;
   
  } else if(tree->kind == Operator) {
    tree->index = tree_index++;

    canonize_tree1(LEFT(tree));
    canonize_tree1(RIGHT(tree));

  }
}

const Node* canonize_tree(const Node *tree) {
  Node* node = (Node*) malloc(sizeof(Node));
  *node = *tree;
  operand_table_index = 0;
  tree_index = 0;
  memset(operand_table, 0, sizeof(Node*) * OPERAND_TABLE_SZ);
  canonize_tree1(node);
  return node;
}

bool compare_operand_no_indexing(const Node *pattern, const Node *target) {
  bool compare_kind = (pattern->operand.kind == operand_any) ||
                      (pattern->operand.kind == target->operand.kind);
  bool compare_hash = (pattern->operand.hash == EMPTY_HASH)
                      || (pattern->operand.hash == target->operand.hash);
  return compare_kind && compare_hash;
}

bool compare_operand(const Node *pattern, const Node *target) {
  bool compare_kind = (pattern->operand.kind == operand_any) ||
    (pattern->operand.kind == target->operand.kind);

  if(pattern->is_pattern) {
    bool compare_index = (pattern->index == target->index);
    return compare_kind && compare_index;
  } else {
    bool compare_hash = (pattern->operand.hash == target->operand.hash);
    return compare_kind && compare_hash;
  }
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
    return compare_operand(LEFT(pattern), LEFT(target)) && compare_operand(RIGHT(pattern), RIGHT(target));
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

bool match_canonized_pattern(const Node *canonized_pattern, const Node *target, match_collector *collector) {

  const Node* canonized_target = canonize_tree(target);

  if(match_pattern1(canonized_pattern, canonized_target)) {
    collect_match(collector, target);
    return true;
  }

  if(canonized_target->kind == Operator) {
    bool match_left = match_canonized_pattern(canonized_pattern, canonized_target->operator.left, collector);
    // if(match_left) collect_match(collector, canonized_target->operator.left);
    bool match_right = match_canonized_pattern(canonized_pattern, canonized_target->operator.right, collector);
    // if(match_right) collect_match(collector, canonized_target->operator.right);
    return (match_left || match_right);				    
  }
  return false;
}

bool match_pattern(const Node *pattern, const Node *target) {
  match_collector collector = {0};
  const Node* canonized_pattern = canonize_tree(pattern);
  match_canonized_pattern(canonized_pattern, target, &collector);
  return collector.size > 0;
}

match_collector match_and_collect_pattern(const Node *pattern, const Node *target) {
  match_collector collector = {0};
  const Node* canonized_pattern = canonize_tree(pattern);
  match_canonized_pattern(canonized_pattern, target, &collector);
  return collector;
}

void collect_match(match_collector *collector, const Node *match) {
  collector->matches[collector->size++] = match;
}
