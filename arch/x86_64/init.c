#include "init.h"
#include "arch_interrupts.h"
#include "gdt.h"
#include "hpet.h"
#include "include/apic.h"
#include "include/pit.h"
#include "include/tsc.h"
#include "interrupts.h"
#include "kernel/kprintf.h"
#include "kernel/mem.h"
#include "kernel/threads.h"
#include "util.h"
#include <stdint.h>

void temp_int_handler(struct int_context *context) {
  kprintf("Page Fault Detected!\n");
  kprintf("P: %x, W: %x, U: %x, I: %x\n", context->error_code & (1 << 0),
          context->error_code & (1 << 1), context->error_code & (1 << 2),
          context->error_code & (1 << 4));

  uint64_t cr2;
  __asm__ volatile("mov %%cr2, %0" : "=r"(cr2));
  kprintf("Page Fault Address: %x\n", cr2);
  panic();
}

void INIT arch_init(void) {
  init_gdt();
  // kprintf("[INIT] Initialized GDT\n");

  init_interrupts();
  enable_interrupts();
  // kprintf("[INIT] Initialized Interrupts\n");

  BSP { register_int_handler(14, (void (*)(void *))temp_int_handler); }

  enable_apic();
  // kprintf("[INIT] Enabled APIC\n");

  BSP {
    init_pit();
    init_tsc();
    // kprintf("[INIT] Initialized PIT and TSC\n");
  }

  BSP {
    init_hpet();
    // kprintf("[INIT] Enabled Main Core Timers\n");
  }

  // kprintf("[INIT] Initialized APIC Timer\n");
  init_apic_timer();
}

extern void __do_stack_switch(void (*entry)(void *new_stack), void *stack,
                              size_t stack_size);

void __switch_stacks(void (*entry)(void *new_stack)) {
  __do_stack_switch(entry, alloc_pages(STACK_ORDER, ZONE_ANY),
                    PAGE_SIZE * (1 << STACK_ORDER));
}
