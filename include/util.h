#ifndef _UTIL_H_
#define _UTIL_H_

#define HLT while (1) {}

#ifdef _x86_64_

#undef HLT
#define HLT __asm__ volatile("hlt");

#endif

#endif

