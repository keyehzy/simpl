#ifndef ASSERT_H
#define ASSERT_H

void __crash(const char* msg, const char* file, unsigned line,
             const char* func);

#define ASSERT(expr)						\
  (expr ?							\
   (void)0							\
   : __crash(#expr, __FILE__, __LINE__, __func__))
#define UNREACHABLE() ASSERT(false)
#define TODO() UNREACHABLE()

#endif
