#include <ps_2.h>
#include <asm.h>

void wait_for_read(void) {
  while (!(inb(PS_2_STATUS_PORT) & 1)) {
  }
}

void wait_for_write(void) {
  while (inb(PS_2_STATUS_PORT) & 2) {
  }
}


