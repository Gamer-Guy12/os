#include "apic.h"
#include "util.h"
#include "x86_64.h"

void panic_handler(void *_) { HLT; }

void panic(void) {
  apic_ipi(IPI_ALL, 0, PANIC_IPI);
  HLT;
  while (1) {
  }
}
