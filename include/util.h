#ifndef _UTIL_H_
#define _UTIL_H_

#include <stdint.h>

#define KB (uint64_t)0x400
#define MB (uint64_t)0x100000
#define GB (uint64_t)0x40000000

#define MAX_8 0xFFull
#define MAX_16 0xFFFFull
#define MAX_32 0xFFFFFFFFull
#define MAX_64 0xFFFFFFFFFFFFFFFFull

#define NORETURN __attribute__((noreturn))

#define LIMINE_REQUEST __attribute__((used, section(".limine_requests")))
#define INIT __attribute__((section(".init.text")))
#define INIT_DATA __attribute__((section(".init.data")))

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
