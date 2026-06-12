#ifndef _x86_64_ARCH_THREADS_H_
#define _x86_64_ARCH_THREADS_H_

#include <stddef.h>
#include <stdint.h>

typedef void *pt_t;

struct registers {
  // Lowest address (pushed last)
  uint64_t rflags;
  uint64_t rbx, rbp, r12, r13, r14, r15;
  // Highest address (pushed first)
  uint64_t rip;
};

struct context {
  union {
    struct registers *regs;
    void *rsp;
  };
};

#endif
