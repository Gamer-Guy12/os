#include "init.h"
#include "arch_interrupts.h"
#include "gdt.h"
#include "hpet.h"
#include "include/apic.h"
#include "include/pit.h"
#include "include/tsc.h"
#include "interrupts.h"
#include "kernel/kprintf.h"
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
  // init_apic_timer();
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
