#ifndef TDD_H
#define TDD_H

#pragma GCC diagnostic ignored "-Wtraditional-conversion"

#include <stdio.h>
#include <string.h>

#define TDD_RESET "\033[0m"
#define TDD_GREEN "\033[32m"
#define TDD_RED "\033[31m"

#define TDD_BUFSIZE 1024
#define TDD_MAXNUM 100

static int TDD_failed_flag = 0;
static int TDD_passed_stat = 0;
static int TDD_failed_stat = 0;

static int TDD_last_error_idx = 0;
static char TDD_error_msg[TDD_MAXNUM][TDD_BUFSIZE];

#define TDD_TEST(name) static void TDD_TEST_##name(void)

#define TDD_BLOCK(block)                                                       \
  do {                                                                         \
    block                                                                      \
  } while (0)

#define TDD_EMIT_ERRORS()                                                      \
  for (int idx = 0; idx <= TDD_last_error_idx; idx++) {                        \
    fprintf(stderr, "%s", TDD_error_msg[idx]);                                 \
  }

#define TDD_CLEANUP()                                                          \
  for (int idx = 0; idx <= TDD_last_error_idx; idx++)                          \
    memset(TDD_error_msg[idx], 0, TDD_BUFSIZE);                                \
  TDD_last_error_idx = 0;						       \
  TDD_failed_flag = 0;

#define TDD_RUN(name)                                                          \
  TDD_BLOCK(                                                                   \
      TDD_TEST_##name(); if (TDD_failed_flag) {                                \
        TDD_failed_stat += 1;                                                  \
	TDD_FAILED(TDD_TEST_##name);					       \
      	TDD_EMIT_ERRORS();						       \
      } else {                                                                 \
        TDD_passed_stat += 1;                                                  \
        TDD_PASSED(TDD_TEST_##name);                                           \
      }									       \
      TDD_CLEANUP();)

#define TDD_FINISH()                                                           \
  fprintf(stderr,                                                              \
          "finished: %s%d%s test passed and %s%d%s failed out of %d\n",        \
          TDD_GREEN, TDD_passed_stat, TDD_RESET, TDD_RED, TDD_failed_stat,     \
          TDD_RESET, TDD_passed_stat + TDD_failed_stat);

#define TDD_PASSED(test)                                                       \
  fprintf(stderr, "%sTest passed%s: %s\n", TDD_GREEN, TDD_RESET, #test)

#define TDD_FAILED(test)                                                       \
  fprintf(stderr, "%sTest failed%s: %s\n", TDD_RED, TDD_RESET, #test)

#define TDD_EQ(x, y)                                                           \
  TDD_BLOCK(TDD_failed_flag = ((x) != (y)); if (TDD_failed_flag) {             \
    snprintf(TDD_error_msg[TDD_last_error_idx++], TDD_BUFSIZE,                 \
             "equality does not hold: %s == %s %s:%s:%d\n", #x, #y, __FILE__,  \
             __func__, __LINE__);                                              \
  })

#define TDD_TRUE(x)                                                            \
  TDD_BLOCK(TDD_failed_flag = !(x); if (TDD_failed_flag) {                     \
    snprintf(TDD_error_msg[TDD_last_error_idx++], TDD_BUFSIZE,                 \
             "expression is not true: %s %s:%s:%d\n", #x, __FILE__, __func__,  \
             __LINE__);                                                        \
  })

#endif
