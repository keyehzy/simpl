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
    const char* foo = "42";
    lexer l = l_new(foo);
    Node *expr = parse(&l);
    mu_assert_int_eq(expr->kind, Operand);
  }

  {
    const char* foo = "foo";
    lexer l = l_new(foo);
    Node *expr = parse(&l);
    mu_assert_int_eq(expr->kind, Operand);
  }

  {
    const char* foo = "#";
    lexer l = l_new(foo);
    Node *expr = parse(&l);
    mu_assert_int_eq(expr->kind, Operand);
    mu_assert_int_eq(expr->operand.kind, operand_any);
  }

  {
    const char* foo = "foo#";
    lexer l = l_new(foo);
    Node *expr = parse(&l);
    mu_assert_int_eq(expr->kind, Operand);
    mu_assert_int_eq(expr->operand.kind, operand_any);
  }

  {
    const char* foo = "1+1";
    lexer l = l_new(foo);
    Node *expr = parse(&l);
    mu_assert_int_eq(expr->kind, Operator);
    mu_assert_int_eq(expr->operator.left->kind, Operand);
    mu_assert_int_eq(expr->operator.right->kind, Operand);
  }

  {
    const char* foo = "1+1*1";
    lexer l = l_new(foo);
    Node *expr = parse(&l);
    mu_assert_int_eq(expr->kind, Operator);
    mu_assert_int_eq(expr->operator.left->kind, Operand);
    mu_assert_int_eq(expr->operator.right->kind, Operator);
  }

  {
    const char* foo = "1*1+1";
    lexer l = l_new(foo);
    Node *expr = parse(&l);
    mu_assert_int_eq(expr->kind, Operator);
    mu_assert_int_eq(expr->operator.left->kind, Operator);
    mu_assert_int_eq(expr->operator.right->kind, Operand);
  }
}

MU_TEST(test_match) {
  {
    const char* pattern = "#";
    lexer plex = l_new(pattern);
    Node *pattern_node = parse(&plex);

    const char* target = "x";
    lexer tlex = l_new(target);
    Node *target_node = parse(&tlex);

    mu_check(match_pattern(pattern_node, target_node));
  }

  {
    const char* pattern = "# + 2";
    lexer plex = l_new(pattern);
    Node *pattern_node = parse(&plex);

    const char* target = "2 + 2";
    lexer tlex = l_new(target);
    Node *target_node = parse(&tlex);

    mu_check(match_pattern(pattern_node, target_node));
  }

  {
    const char* pattern = "x#";
    lexer plex = l_new(pattern);
    Node *pattern_node = parse(&plex);

    const char* target = "x + x";
    lexer tlex = l_new(target);
    Node *target_node = parse(&tlex);

    mu_check(match_pattern(pattern_node, target_node));
  }

}

MU_TEST_SUITE(test_suite) {
  MU_RUN_TEST(test_lexer);
  MU_RUN_TEST(test_parser);
  MU_RUN_TEST(test_match);
}

int main(void) {
  MU_RUN_SUITE(test_suite);
  MU_REPORT();
  return MU_EXIT_CODE;
}

