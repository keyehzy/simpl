#include "tdd.h"

TDD_TEST(simple) { TDD_TRUE(0); }
TDD_TEST(simple1) { TDD_TRUE(1); }
TDD_TEST(simple2) { TDD_EQ(1, 2); }
TDD_TEST(simple3) {
  TDD_TRUE(1 == 2);
  TDD_TRUE(2 == 3);
}

int main(void) {
  TDD_RUN(simple);
  TDD_RUN(simple1);
  TDD_RUN(simple2);
  TDD_RUN(simple3);
  TDD_FINISH();
  return 0;
}
