#ifndef _x86_64_ASM_H_
#define _x86_64_ASM_H_

#include <stdint.h>

#define OUTB(port, val)                                                        \
  __asm__ volatile("outb %b0, %w1" ::"a"(val), "Nd"(port) : "memory");
#define OUTW(port, val)                                                        \
  __asm__ volatile("outw %w0, %w1" ::"a"(val), "Nd"(port) : "memory");
#define OUTL(port, val)                                                        \
  __asm__ volatile("outl %0, %w1" ::"a"(val), "Nd"(port) : "memory");

#define IO_WAIT() OUTB(0x80, 0)

static inline uint8_t inb(uint16_t port) {
  uint8_t ret;
  __asm__ volatile("inb %w1, %b0" : "=a"(ret) : "Nd"(port) : "memory");
  return ret;
}

static inline uint8_t inw(uint16_t port) {
  uint8_t ret;
  __asm__ volatile("inw %w1, %w0" : "=a"(ret) : "Nd"(port) : "memory");
  return ret;
}

static inline uint8_t inl(uint16_t port) {
  uint8_t ret;
  __asm__ volatile("inl %w1, %l0" : "=a"(ret) : "Nd"(port) : "memory");
  return ret;
}

static inline void cpuid(int code, uint32_t *a, uint32_t *d) {
  __asm__ volatile("cpuid" : "=a"(*a), "=d"(*d) : "0"(code) : "ebx", "ecx");
}

static inline void cpuid_all(int code, uint32_t *a, uint32_t *b, uint32_t *c, uint32_t *d) {
  __asm__ volatile("cpuid" : "=a"(*a), "=d"(*d), "=b"(*b), "=c"(*c) : "0"(code));
}

#define WRMSR(msr, value)                                                      \
  __asm__ volatile("wrmsr" ::"c"(msr), "a"((uint64_t)(value) & 0xFFFFFFFF),    \
                   "d"((uint64_t)(value) >> 32))

static inline uint64_t rdmsr(uint64_t msr) {
  uint32_t low, high;
  __asm__ volatile("rdmsr" : "=a"(low), "=d"(high) : "c"(msr));
  return ((uint64_t)high << 32) | low;
}

#endif
