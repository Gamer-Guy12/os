#include "interrupts.h"
#include <stddef.h>

static int interrupt_count = 0;

void _disable_interrupts(void) {
  __asm__ volatile("cli" ::: "memory");
  if (interrupt_count < 0) interrupt_count = 0;
  interrupt_count++;
}

void _enable_interrupts(void) {
  interrupt_count--;
  if (interrupt_count <= 0) {
    __asm__ volatile("cli" ::: "memory");
  }
}
