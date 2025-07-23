/// Much of the code in this file is taken from the osdev wiki
/// https://wiki.osdev.org/Inline_Assembly/Examples#WRMSR

#ifndef X86_64_IO_H
#define X86_64_IO_H

#include <stddef.h>
#include <stdint.h>

static inline void outb(uint16_t port, uint8_t byte) {
  __asm__ volatile("outb %b0, %w1" : : "a"(byte), "Nd"(port) : "memory");
}

static inline uint8_t inb(uint16_t port) {
  uint8_t ret;
  __asm__ volatile("inb %w1, %b0" : "=a"(ret) : "Nd"(port) : "memory");

  return ret;
}

static inline void io_wait(void) { outb(0x80, 0); }

/// eax is the low bits and edx is the high
static inline void wrmsr(uint64_t msr, uint64_t value) {
  uint32_t low = value & 0xFFFFFFFF;
  uint32_t high = value >> 32;
  __asm__ volatile("wrmsr" : : "c"(msr), "a"(low), "d"(high));
}

static inline uint64_t rdmsr(uint64_t msr) {
  uint32_t low, high;
  __asm__ volatile("rdmsr" : "=a"(low), "=d"(high) : "c"(msr));
  return ((uint64_t)high << 32) | low;
}

static inline void cpuid(int code, uint32_t *a, uint32_t *d) {
  __asm__ volatile("cpuid" : "=a"(*a), "=d"(*d) : "0"(code) : "ebx", "ecx");
}

static inline void interrupt(uint8_t interrupt) {
  __asm__ volatile("int %0" ::"i"(interrupt) :);
}

/// @return 1 if sucess and 0 if failure
///
/// @param dest this is a pointer to 16 bytes of contiguous memory which is
/// compared
///
/// @param value this is the value that both elements are checked against
///
/// @param new this is the new value that will be loaded into memory
///
/// It checks that both values in a 16 byte contiguous range of memory are equal
/// to value and if so returns 1 and loads new into all 128 bytes
uint64_t cmpxchg16b(void *dest, uint64_t value, uint64_t new);

#endif
