#include "minunit.h"

#include "lex.h"
#include "simpl.h"
#include "match.h"

MU_TEST(test_lexer) {
  {
    const char* foo = "Hello, world\n";
    stream s = s_new(foo);
    mu_assert_int_eq(s_peek(&s)[0], 'H');
    s_skip(&s);
    mu_assert_int_eq(s_peek(&s)[0], 'e');
  }

  {
    const char* foo = "e";
    stream s = s_new(foo);
    mu_assert_int_eq(s_peek(&s)[0], 'e');
    s_skip(&s);
    mu_check(s_is_eof(&s));
  }

  {
    const char* foo = " \v\r\t\n\fe";
    stream s = s_new(foo);
    skip_ws(&s);    
    mu_assert_int_eq(s_peek(&s)[0], 'e');
  }

  {
    const char* foo = " \v\r\t\n\fe";
    stream s = s_new(foo);
    skip_ws(&s);    
    mu_assert_int_eq(s_peek(&s)[0], 'e');
  }

  {
    const char* foo = "bar";
    stream s = s_new(foo);
    token t = next_token(&s);    
    mu_assert_int_eq(t.type, tk_identifier);
    mu_check(s_is_eof(&s));
  }

  {
    const char* foo = "42";
    stream s = s_new(foo);
    token t = next_token(&s);    
    mu_assert_int_eq(t.type, tk_number_literal);
    mu_check(s_is_eof(&s));
  }

  {
    const char* foo = "#Something";
    stream s = s_new(foo);
    token t = next_token(&s);
    mu_assert_int_eq(t.type, tk_identifier_pattern);
    mu_check(s_is_eof(&s));
  }

  {
    const char* foo = "#";
    stream s = s_new(foo);
    token t = next_token(&s);
    mu_assert_int_eq(t.type, tk_identifier_pattern);
    mu_check(s_is_eof(&s));
  }

  {
    const char* foo[] = {"+", "-", "*", "/"};
    const token_type tk_types[] = {tk_plus, tk_minus, tk_times, tk_div};
    for(int i = 0; i < 4; i++) {
      stream s = s_new(foo[i]);
      token t = next_token(&s);    
      mu_assert_int_eq(t.type, tk_types[i]);
      mu_check(s_is_eof(&s));
    }
  }
}

MU_TEST(test_parser) {
  {
    Node *expr = parse_expression("42");
    mu_assert_int_eq(expr->kind, Operand);
  }

  {
    Node *expr = parse_expression("foo");
    mu_assert_int_eq(expr->kind, Operand);
  }

  {
    Node *expr = parse_expression("#");
    mu_assert_int_eq(expr->kind, Any);
    mu_assert_int_eq(expr->operand.kind, operand_any);
  }

  {
    Node *expr = parse_expression("foo#");
    mu_assert_int_eq(expr->kind, Operand);
    mu_assert_int_eq(expr->operand.kind, operand_any);
  }

  {
    Node *expr = parse_expression("1+1");
    mu_assert_int_eq(expr->kind, Operator);
    mu_assert_int_eq(expr->operator.left->kind, Operand);
    mu_assert_int_eq(expr->operator.right->kind, Operand);
  }

  {
    Node *expr = parse_expression("1+1*1");
    mu_assert_int_eq(expr->kind, Operator);
    mu_assert_int_eq(expr->operator.left->kind, Operand);
    mu_assert_int_eq(expr->operator.right->kind, Operator);
  }

  {
    Node *expr = parse_expression("1*1+1");
    mu_assert_int_eq(expr->kind, Operator);
    mu_assert_int_eq(expr->operator.left->kind, Operator);
    mu_assert_int_eq(expr->operator.right->kind, Operand);
  }
}

MU_TEST(test_match) {
  {
    Node *pattern_node = parse_expression("#");
    Node *target_node = parse_expression("x");
    mu_check(match_pattern(pattern_node, target_node));
  }

  {
    Node *pattern_node = parse_expression("# + 2");
    Node *target_node = parse_expression("2 + 2");
    mu_check(match_pattern(pattern_node, target_node));
  }

  {
    Node *pattern_node = parse_expression("x#");
    Node *target_node = parse_expression("x + x");
    mu_check(match_pattern(pattern_node, target_node));
  }

  {
    Node *pattern_node = parse_expression("x# + y#");
    Node *target_node = parse_expression("1 + 2");
    mu_check(match_pattern(pattern_node, target_node));
  }

  {
    Node *pattern_node = parse_expression("x# + y# * #z");
    Node *target_node = parse_expression("1 + 2 * 3 + 4");
    mu_check(match_pattern(pattern_node, target_node));
  }
}

MU_TEST(test_canonize_tree) {
  {
    Node *tree = parse_expression("1 + 2");
    canonize_tree(tree);
    mu_assert_int_eq(tree->index, 0);
    mu_assert_int_eq(LEFT(tree)->index, 1);
    mu_assert_int_eq(RIGHT(tree)->index, 2);
  }

  {
    Node *tree = parse_expression("2 + 2");
    canonize_tree(tree);
    mu_assert_int_eq(tree->index, 0);
    mu_assert_int_eq(LEFT(tree)->index, 1);
    mu_assert_int_eq(RIGHT(tree)->index, 1);
  }

  {
    Node *tree = parse_expression("x# + y#");
    canonize_tree(tree);
    mu_assert_int_eq(tree->index, 0);
    mu_assert_int_eq(LEFT(tree)->index, 1);
    mu_assert_int_eq(RIGHT(tree)->index, 2);
  }

  {
    Node *tree = parse_expression("x# + x#");
    canonize_tree(tree);
    mu_assert_int_eq(tree->index, 0);
    mu_assert_int_eq(LEFT(tree)->index, 1);
    mu_assert_int_eq(RIGHT(tree)->index, 1);
  }
}

MU_TEST_SUITE(test_suite) {
  MU_RUN_TEST(test_lexer);
  MU_RUN_TEST(test_parser);
  MU_RUN_TEST(test_match);
  MU_RUN_TEST(test_canonize_tree);
}

int main(void) {
  MU_RUN_SUITE(test_suite);
  MU_REPORT();
  return MU_EXIT_CODE;
}

