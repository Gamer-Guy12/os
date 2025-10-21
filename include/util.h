#ifndef _UTIL_H_
#define _UTIL_H_

#include <stdint.h>

#define KB (uint64_t)0x400
#define MB (uint64_t)0x100000
#define GB (uint64_t)0x40000000

#define HLT                                                                    \
  while (1) {                                                                  \
  }

#define LIMINE_REQUEST __attribute__((used, section(".limine_requests")))

#ifdef _x86_64_

#undef HLT
#define HLT __asm__ volatile("hlt");

#endif

#endif
