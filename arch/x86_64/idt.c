#include "idt.h"
#include "gdt.h"
#include "kernel/cores.h"
#include "kernel/kprintf.h"
#include "util.h"
#include <stddef.h>
#include <stdint.h>

#define IDT_ENTRIES 256
typedef void (*handler_t)(void *);

// There are 256 entries in the idt
struct int_descriptor idt[IDT_ENTRIES];
extern uint64_t idt_funcs[IDT_ENTRIES];
static handler_t handlers[IDT_ENTRIES];

void register_interrupt(void (*handler)(void *), int interrupt) {
  // This prevents things from being reordered before the handler they need is
  // in place
  __atomic_store_n(&handlers[interrupt], handler, __ATOMIC_RELEASE);
}

void common_handler(struct int_context *context) {
  if (handlers[context->int_number] != NULL) {
    handlers[context->int_number](context);
  } else {
    kprintf("No handler for interrupt %u on core %u\n", context->int_number,
            get_core_id());
    panic();
  }
}

void init_idt(void) {
  BSP {
    for (int i = 0; i < IDT_ENTRIES; i++) {
      idt[i].offset_0 = idt_funcs[i] & MAX_16;
      idt[i].offset_1 = (idt_funcs[i] >> 16) & MAX_16;
      idt[i].offset_2 = (idt_funcs[i] >> 32) & MAX_32;

      idt[i].segment = GDT_CODE0;

      // Use the first ist
      idt[i].ist = 1;
      // IDT_INTERRUPT means that it disables interrupts during the handler
      idt[i].gate_type = IDT_INTERRUPT;
      idt[i].dpl = 0;
      idt[i].present = 1;
    }
  }

  idt_descriptor_t ptr = {.size = sizeof(idt) - 1, .addr = (uint64_t)idt};
  __asm__ volatile("lidt (%0)" ::"r"(&ptr) : "memory");
}
