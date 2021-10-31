#include "tdd/tdd.h"

#include "lex.h"

TDD_TEST(simple) {
  TDD_TRUE(1);
}

TDD_TEST(lexer) {
  {
    const char* foo = "Hello, world\n";
    stream s = s_new(foo);
    TDD_EQ(s_peek(&s)[0], 'H');
    s_skip(&s);
    TDD_EQ(s_peek(&s)[0], 'e');
  }

  {
    const char* foo = "e";
    stream s = s_new(foo);
    TDD_EQ(s_peek(&s)[0], 'e');
    s_skip(&s);
    TDD_TRUE(s_is_eof(&s));
  }

  {
    const char* foo = " \v\r\t\n\fe";
    stream s = s_new(foo);
    skip_ws(&s);    
    TDD_EQ(s_peek(&s)[0], 'e');
  }

  {
    const char* foo = " \v\r\t\n\fe";
    stream s = s_new(foo);
    skip_ws(&s);    
    TDD_EQ(s_peek(&s)[0], 'e');
  }

  {
    const char* foo = "bar";
    stream s = s_new(foo);
    token t = next_token(&s);    
    TDD_EQ(t.type, tk_identifier);
    TDD_TRUE(s_is_eof(&s));
  }

  {
    const char* foo = "42";
    stream s = s_new(foo);
    token t = next_token(&s);    
    TDD_EQ(t.type, tk_number_literal);
    TDD_TRUE(s_is_eof(&s));
  }

  {
    const char* foo[] = {"+", "-", "*", "/"};
    for(int i = 0; i < 4; i++) {
      stream s = s_new(foo[i]);
      token t = next_token(&s);    
      TDD_EQ(t.type, tk_number_literal);
      TDD_TRUE(s_is_eof(&s));
    }
  }

}


int main(void) {
  TDD_RUN(simple);
  TDD_RUN(lexer);
  TDD_FINISH();
  return 0;
}

