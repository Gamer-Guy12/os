#include "arch_interrupts.h"
#include "kernel/kprintf.h"
#include "util.h"
#include <stddef.h>
#include <stdint.h>

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

static struct idt_descriptor idt[256];

typedef void (*handler_t)(void *);
static handler_t handlers[256] = {NULL};

#include "./idt_gen.h"

void init_interrupts(void) {
  fill_idt();

  idtptr_t ptr = {.size = sizeof(idt) - 1, .addr = (uint64_t)idt};
  __asm__ volatile("lidt (%0)" ::"r"(&ptr) : "memory");
}

void register_int_handler(uint16_t interrupt, void (*handler)(void *)) {
  handlers[interrupt] = handler;
}

void common_handler(struct int_context *context) {
  if (handlers[context->int_number] != NULL) {
    handlers[context->int_number](context);
  } else {
    kprintf("No handler for interrupt %u\n", context->int_number);
    panic();
  }
}
