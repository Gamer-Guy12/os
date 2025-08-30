/// Much of the code in this file is taken from the osdev wiki
/// https://wiki.osdev.org/Inline_Assembly/Examples#WRMSR

#ifndef X86_64_IO_H
#define X86_64_IO_H

#include <decls.h>
#include <stdbool.h>
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

static inline uint64_t WUNUSED rdmsr(uint64_t msr) {
  uint32_t low, high;
  __asm__ volatile("rdmsr" : "=a"(low), "=d"(high) : "c"(msr));
  return ((uint64_t)high << 32) | low;
}

static inline void cpuid(int code, uint32_t *a, uint32_t *d, uint32_t *c,
                         uint32_t *b) {
  __asm__ volatile("cpuid"
                   : "=a"(*a), "=d"(*d), "=c"(*c), "=b"(*b)
                   : "0"(code));
}

static inline void interrupt(uint8_t interrupt) {
  __asm__ volatile("int %0" ::"i"(interrupt) :);
}

static inline size_t WUNUSED coreid(void) {
  size_t coreid = 0;
  __asm__ volatile("mov $1, %%eax; cpuid; shrl $24, %%ebx;"
                   : "=b"(coreid)::"rax");
  return coreid;
}

static inline size_t WUNUSED rdtsc(void) {
  size_t a = 0, d = 0;
  __asm__ volatile("rdtsc" : "=a"(a), "=d"(d));

  return a | (d << 32);
}

void __cli(void);
void __sti(void);

#define FS_MSR 0xC0000100

#define MFENCE __asm__ volatile("mfence" ::: "memory")
#define HLT __asm__ volatile("hlt")
#define TCB ((TCB_t *)(rdmsr(FS_MSR)))
#define CLI __cli()
#define STI __sti()
#define DIV0 __asm__ volatile("div %%rcx" ::"c"(0))
#define ASM(code) __asm__ volatile(code)

/// Returns original value
#define ATOMIC_INC(num) __atomic_fetch_add(&num, 1, __ATOMIC_RELEASE)
#define ATOMIC_DEC(num) __atomic_fetch_sub(&num, 1, __ATOMIC_RELEASE)

/// Returns original value
#define ATOMIC_FADD(num, val) __atomic_fetch_add(&num, val, __ATOMIC_RELEASE)
#define ATOMIC_FSUB(num, val) __atomic_fetch_sub(&num, val, __ATOMIC_RELEASE)

/// Returns if success
#define CAS(num, old, new) __sync_bool_compare_and_swap(&num, old, new)

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
