#ifndef LEX_H
#define LEX_H

#include "loc.h"

#include <stdbool.h>

#define ENUMERATE_NUMBERS(O)						\
  O('0') O('1') O('2') O('3') O('4') O('5') O('6') O('7') O('8')

#define ENUMERATE_OPERATORS(O)			\
  O('+') O('-') O('*') O('/')

#define ENUMERATE_CHARACTERS_LOWERCASE(O)				\
  O('a') O('b') O('c') O('d') O('e') O('f') O('g') O('h') O('i')	\
  O('j') O('k') O('l') O('m') O('n') O('o') O('p') O('q') O('r')	\
  O('s') O('t') O('u') O('v') O('x') O('y') O('z')

#define ENUMERATE_CHARACTERS_UPPERCASE(O)				\
  O('A') O('B') O('C') O('D') O('E') O('F') O('G') O('H') O('I')	\
  O('J') O('K') O('L') O('M') O('N') O('O') O('P') O('Q') O('R')	\
  O('S') O('T') O('U') O('V') O('X') O('Y') O('Z')

#define CASE(num) case num:
#define CASE_NUMBERS ENUMERATE_NUMBERS(CASE)
#define CASE_OPERATORS ENUMERATE_OPERATORS(CASE)
#define CASE_CHARACTERS				\
  ENUMERATE_CHARACTERS_LOWERCASE(CASE)		\
    ENUMERATE_CHARACTERS_UPPERCASE(CASE)

typedef struct {
  const char *buffer;
} stream;

stream s_new(const char*);
void s_skip(stream*);
const char* s_peek(stream*);
bool s_is_eof(stream*);

typedef enum {
  tk_identifier,
  tk_number_literal,
  tk_plus,
  tk_minus,
  tk_times,
  tk_div,
  tk_eof,
} token_type;

typedef struct {
  token_type type;
  location loc;
} token;

typedef struct {
  stream s;
  token last_token;
} lexer;

lexer l_new(const char*);
void l_skip(lexer*);
token l_peek(lexer*);

void skip_ws(stream *);
token next_token(stream *); 

#endif
