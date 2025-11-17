#include "init.h"
#include "./init.h"
#include "gdt.h"
#include "kernel/kprintf.h"

void arch_init(void) {
  init_gdt();
  kprintf("[INIT] Initialized GDT\n");

  init_interrupts();
  kprintf("[INIT] Initialized Interrupts\n");
}
