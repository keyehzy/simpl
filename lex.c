#include "lex.h"
#include "assert.h"

#include <stddef.h>
#include <string.h>

stream s_new(const char* buffer) {
  return (stream) {.buffer = buffer};
}

void s_skip(stream* s) {
  s->buffer += 1;
}

const char* s_peek(stream* s) {
  return s->buffer;
}

bool s_is_eof(stream* s) {
  return s_peek(s)[0] == '\0';
}

void skip_ws(stream *s) {
  const char *it = s_peek(s);
  while (it[0] == ' ' || it[0] == '\t' || it[0] == '\f' ||
	 it[0] == '\v' || it[0] == '\n' || it[0] == '\r') {
    it += 1;
  }
  s->buffer = it;
}

const char* eat_number(const char* begin) {
  const char* it = begin + 1;
  while(1) {
    switch(it[0]) {
      CASE_NUMBERS
	it++;
        break;
    default:
      return it;
    }     
  }
}

token parse_number(stream *s) {
  const char* begin = s_peek(s);
  const char* end = eat_number(begin);
  s->buffer = end;
  location loc = (location) {.begin = begin, .end = end};
  return (token) {.type = tk_number_literal, .loc = loc};  
}

token_type get_operator_type(int o) {
  switch(o) {
  case '+':
    return tk_plus;
  case '-':
    return tk_minus;
  case '*':
    return tk_times;
  case '/':
    return tk_div;
  case '@':
    return tk_anonymous_operator_pattern;
  default:
    UNREACHABLE();
  }
  return (token_type) {0};
}

token parse_operator(stream *s) {
  const char* begin = s_peek(s);
  token_type type = get_operator_type(begin[0]);
  location loc = (location) {.begin = begin, .end = begin + 1};
  s_skip(s);
  return (token) {.type = type, .loc = loc};  
}

const char* eat_word(const char* begin) {
  const char* it = begin;
  while(1) {
    switch(it[0]) {
      CASE_NUMBERS
      CASE_CHARACTERS
	it++;
        break;
    default:
      return it;
    }
  }
}

token parse_identifier(stream *s) {
  const char* begin = s_peek(s);
  const char* end = eat_word(begin);
  s->buffer = end;
  location loc = (location) {.begin = begin, .end = end};
  return (token) {.type = tk_identifier, .loc = loc};  
}

token parse_anonymous_identifier_pattern(stream *s) {
  const char* begin = s_peek(s);
  const char* end = eat_word(begin + 1);
  s->buffer = end;
  location loc = (location) {.begin = begin, .end = end};
  return (token) {.type = tk_anonymous_identifier_pattern, .loc = loc};
}

token parse_anonymous_operator_pattern(stream *s) {
  return parse_operator(s);
}

token next_token(stream *s) {
  skip_ws(s);
  switch(s_peek(s)[0]) {
  CASE_NUMBERS
    return parse_number(s);
  CASE_OPERATORS
    return parse_operator(s);
  CASE_CHARACTERS
    return parse_identifier(s);
  case '#':
    return parse_anonymous_identifier_pattern(s);
  case '@':
    return parse_anonymous_operator_pattern(s);
  case '\0':
    return (token) {.type = tk_eof, .loc = {0}};
  default:
    UNREACHABLE();
  }
  return (token) {0};
}

lexer l_new(const char* buffer) {
  lexer lex = {0};
  lex.s = s_new(buffer);
  lex.last_token = next_token(&lex.s);
  return lex;
}

void l_skip(lexer* lex) {
  lex->last_token = next_token(&lex->s);
}

token l_peek(lexer* l) {
  return l->last_token;
}
