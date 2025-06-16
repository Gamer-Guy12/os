#include "libk/kio.h"
#include <interrupts.h>
#include <stdint.h>

void init_interrupts(void) {
  register_handlers();
  enable_idt_gate(0x50);
  load_idt();

  kio_printf("Calling\n");
  __asm__ volatile("int $0x50");
}
