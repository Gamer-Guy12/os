#include "apic.h"
#include "kernel/cores.h"
#include "kernel/kprintf.h"
#include "util.h"
#include "x86_64.h"

void panic_handler(void *_) {
  kprintf("Panic recieved on core: %u\n", get_core_id());
  HLT;
}

void panic(void) {
  apic_ipi(IPI_ALL, 0, PANIC_IPI);
  HLT;
  while (1) {
  }
}
