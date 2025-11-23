#ifndef _x86_64_ARCH_THREADS_H_
#define _x86_64_ARCH_THREADS_H_

#include <stdint.h>
#include <stddef.h>

struct registers {
  uint64_t rflags;
  uint64_t rbx, rbp, r12, r13, r14, r15;
  uint64_t rip;
};

struct context {
  union {
    void *rsp;
    struct registers *regs;
  };
};

typedef void *pt_t;

#define PT_NULL NULL

#endif
