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

void INIT arch_init_single(void) {
  init_gdt();
  kprintf("[INIT] Initialized GDT\n");

  init_interrupts();
  enable_interrupts();
  kprintf("[INIT] Initialized Interrupts\n");

  init_pit();
  init_tsc();
  kprintf("[INIT] Initialized PIT and TSC\n");

  enable_apic();
  init_apic_timer();
  kprintf("[INIT] Enabled APIC\n");

  init_hpet();
  kprintf("[INIT] Enabled Main Core Timers\n");
}

void INIT arch_init(void) {
  init_gdt();
  kprintf("[INIT] Initialized GDT\n");

  init_interrupts();
  kprintf("[INIT] Initialized Interrupts\n");

  enable_apic();
  kprintf("[INIT] Enabled APIC\n");

  init_apic_timer();
  kprintf("[INIT] Initialized Timers\n");
}

extern void __do_stack_switch(void (*entry)(void *new_stack), void *stack, size_t stack_size);

void __switch_stacks(void (*entry)(void *new_stack)) {
  __do_stack_switch(entry, alloc_pages(STACK_ORDER, ZONE_ANY), PAGE_SIZE * (1 << STACK_ORDER));
}
