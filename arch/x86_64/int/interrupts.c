#include "interrupts.h"
#include "kernel/kprintf.h"
#include "util.h"

static int interrupt_count = 0;

void disable_interrupts(void) {
  __asm__ volatile("cli" ::: "memory");
  interrupt_count++;
}

// Since this only happens on one core at a time its fine to not use atomics
void enable_interrupts(void) {
  interrupt_count--;

  if (interrupt_count == 0)
    __asm__ volatile("sti" ::: "memory");
  else if (interrupt_count < 0) {
    kprintf("Too many interrupt reenables\n");
    panic();
  }
}
