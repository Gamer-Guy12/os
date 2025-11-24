#include "init.h"
#include "gdt.h"
#include "include/apic.h"
#include "util.h"
#include "arch_interrupts.h"
#include "kernel/kprintf.h"

void INIT arch_init(void) {
  init_gdt();
  kprintf("[INIT] Initialized GDT\n");

  init_interrupts();
  kprintf("[INIT] Initialized Interrupts\n");

  enable_apic();
  kprintf("[INIT] Enabled APIC\n");
}
