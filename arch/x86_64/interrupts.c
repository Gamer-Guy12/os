#include "arch_interrupts.h"
#include "kernel/cores.h"
#include "kernel/kprintf.h"
#include "util.h"
#include <stddef.h>
#include <stdint.h>

CLS(int, interrupt_count);

void disable_interrupts(void) {
  __asm__ volatile("cli" ::: "memory");
  int *this_int = GET_CLS(interrupt_count);
  if (CHECK_CLS(interrupt_count)) {
    (*this_int)++;
  }
}

void temp_disable_interrupts(void) {
  __asm__ volatile("cli" ::: "memory");
}

// Since this only happens on one core at a time its fine to not use atomics
void enable_interrupts(void) {
  int *this_int = GET_CLS(interrupt_count);
  if (CHECK_CLS(interrupt_count))
    (*this_int)--;
  else {
    __asm__ volatile("sti" ::: "memory");
    return;
  }

  if (*this_int <= 0) {
    *this_int = 0;
    __asm__ volatile("sti" ::: "memory");
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
    kprintf("No handler for interrupt %u on core %u\n", context->int_number, get_core_id());
    panic();
  }
}
