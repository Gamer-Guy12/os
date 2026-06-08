#ifndef _UTIL_H_
#define _UTIL_H_

#include "kernel/cores.h"
#include <stdint.h>

#define KB (uint64_t)0x400
#define MB (uint64_t)0x100000
#define GB (uint64_t)0x40000000

#define MAX_8 0xFFull
#define MAX_16 0xFFFFull
#define MAX_32 0xFFFFFFFFull
#define MAX_64 0xFFFFFFFFFFFFFFFFull

#define NORETURN __attribute__((noreturn))
#define USED __attribute((used))

#define LIMINE_REQUEST __attribute__((used, section(".limine_requests")))

#define SASSERT _Static_assert

#define BSP if (is_bsp())
#define AP if (!is_bsp())

#define MEMB() __atomic_thread_fence(__ATOMIC_SEQ_CST)
#define WMEMB() __atomic_thread_fence(__ATOMIC_RELEASE)
#define RMEMB() __atomic_thread_fence(__ATOMIC_ACQUIRE)

enum init_type {
  // Called as soon as the memory manager goes up (run only on bsp)
  // Use stuff like _kprintf
  CALL_MEM,
  // Called as sson as CLS is set up (run on all threads)
  // still use the early versions
  CALL_CLS,
  // Run after threading is set up (run on all cores)
  // Later versions are allowed
  CALL_THREADS,
  // Run after all initialization is done (run on all cores) (no interrupts
  // though)
  CALL_LATE,
  // This is run when everything is like the system is running
  CALL_FINAL
};

struct init_entry {
  void (*ptr)(void);
  uint32_t type;
#ifdef _x86_64_
  uint32_t padding;
#endif
};

// If the function is static declare it as USED
#define INITFUNC(func, call_type)                                              \
  __attribute__((used,                                                         \
                 section(".init_func"))) struct init_entry func##_call = {     \
      .ptr = func, .type = call_type}

void do_calls(uint32_t type);

#ifdef _x86_64_
#define HLT __asm__ volatile("hlt");
#else
#define HLT                                                                    \
  while (1) {                                                                  \
  }
#endif

NORETURN static inline void panic(void) {
  HLT;
  while (1) {
  }
}

#endif
