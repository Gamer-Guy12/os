#include "interrupts.h"
#include "kernel/cores.h"
#include <stddef.h>

static int interrupt_count = 0;
CLS(int, int_count);

void _disable_interrupts(void) {
  __asm__ volatile("cli" ::: "memory");
  if (interrupt_count < 0)
    interrupt_count = 0;
  interrupt_count++;
}

void _enable_interrupts(void) {
  interrupt_count--;
  if (interrupt_count <= 0) {
    __asm__ volatile("sti" ::: "memory");
  }
}

void disable_interrupts(void) {
  __asm__ volatile("cli" ::: "memory");
  int *core_ints = GET_CLS(int_count);
  if (*core_ints < 0)
    *core_ints = 0;
  (*core_ints)++;
}

void enable_interrupts(void) {
  int *core_ints = GET_CLS(int_count);
  (*core_ints)--;
  if (*core_ints <= 0)
    __asm__ volatile("sti" ::: "memory");
}
