#ifndef _x86_64_ARCH_THREADS_H_
#define _x86_64_ARCH_THREADS_H_

#include <stdint.h>

struct registers {
  uint64_t rflags;
  uint64_t rbx, rbp, r12, r13, r14, r15;
  uint64_t rip;
};

struct context {
  // Stack contains other registers and return address
  union {
    void *rsp;
    struct registers *regs;
  };
  uint64_t fs, gs;
};

typedef void *pt_t;

#endif
