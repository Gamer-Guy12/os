#include <interrupts.h>
#include <stdint.h>

void init_interrupts(void) {
  create_handlers();
  register_handlers();
  load_idt();

  __asm__ volatile("sti");
}
