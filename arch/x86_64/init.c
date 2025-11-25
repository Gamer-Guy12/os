#include "init.h"
#include "gdt.h"
#include "include/apic.h"
#include "include/pit.h"
#include "include/tsc.h"
#include "util.h"
#include "arch_interrupts.h"
#include "kernel/kprintf.h"

void INIT arch_init_single(void) {
  init_gdt();
  kprintf("[INIT] Initialized GDT\n");

  init_interrupts();
  kprintf("[INIT] Initialized Interrupts\n");

  init_pit();
  init_tsc();
  kprintf("[INIT] Initialized PIT and TSC\n");

  enable_apic();
  init_apic_timer();
  kprintf("[INIT] Enabled APIC\n");
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
