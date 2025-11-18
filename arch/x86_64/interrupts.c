#include "arch_interrupts.h"
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

void common_handler(struct int_context *context) {
  while (1) {}
}

static struct idt_descriptor idt[256];

#include "./idt_gen.h"

void init_interrupts(void) {
  fill_idt();

  idtptr_t ptr = { .size = sizeof(idt) - 1, .addr = (uint64_t)idt };
  __asm__ volatile("lidt (%0)" :: "r"(&ptr) : "memory");
}
