#ifndef _x86_64_ASM_H_
#define _x86_64_ASM_H_

#include <stdint.h>

#define OUTB(port, val) __asm__ volatile("outb %b0, %w1" :: "a"(val), "Nd"(port) : "memory");
#define OUTW(port, val) __asm__ volatile("outw %w0, %w1" :: "a"(val), "Nd"(port) : "memory");
#define OUTL(port, val) __asm__ volatile("outl %0, %w1" :: "a"(val), "Nd"(port) : "memory");

#define IO_WAIT() outb(0x80, 0)

#endif

