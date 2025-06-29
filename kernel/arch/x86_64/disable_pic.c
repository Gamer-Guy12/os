#include <asm.h>
#include <pic.h>

/// No macros but this should basically be what is on the osdev wiki for
/// programming the pic initialization and also disablign it
void disable_pic(void) {
  outb(0x20, 0x10 | 0x1);
  io_wait();
  outb(0xA0, 0x10 | 0x1);
  io_wait();

  outb(0x21, 0x20);
  io_wait();
  outb(0xA1, 0x28);
  io_wait();

  outb(0x21, 4);
  io_wait();
  outb(0xA1, 2);
  io_wait();

  outb(0x21, 0x1);
  io_wait();
  outb(0xA1, 0x1);
  io_wait();

  outb(0x21, 0xFF);
  outb(0xA1, 0xFF);
}
