#include <interrupts.h>
#include <stdint.h>

void init_interrupts(void) {
  register_handlers();
  load_idt();
}
