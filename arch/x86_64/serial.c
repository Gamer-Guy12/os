#include "asm.h"
#include "kernel/console.h"
#include <stdbool.h>

#define PORT 0x3F8
bool working = true;

void serial_init(void) {
  OUTB(PORT + 1, 0x00);
  OUTB(PORT + 3, 0x80);
  OUTB(PORT + 0, 0x03);
  OUTB(PORT + 1, 0x00);
  OUTB(PORT + 3, 0x03);
  OUTB(PORT + 2, 0xC7);
  OUTB(PORT + 4, 0x0B);
  OUTB(PORT + 4, 0x1E);
  OUTB(PORT + 0, 0xAE);

  if (inb(PORT + 0) != 0xAE) {
    working = false;
  }

  OUTB(PORT + 4, 0x0F);
}

void serial_writechar(char c) {
  if (!working)
    return;

  while ((inb(PORT + 5) & 0x20) == 0)
    ;

  OUTB(PORT, c);
}

void serial_clear(void) {
  char seq[] = "\033[2J";

  for (int i = 0; seq[i] != '\0'; i++) {
    serial_writechar(seq[i]);
  }
}
