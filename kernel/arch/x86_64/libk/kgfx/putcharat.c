#include "./vga.h"
#include <libk/kgfx.h>
#include <x86_64.h>

static volatile uint16_t *terminalBuffer =
    (volatile uint16_t *)(0xb8000 + KERNEL_CODE_OFFSET);

void kgfx_putcharat(uint16_t x, uint16_t y, char c) {

  if (x >= VGA_WIDTH || y >= VGA_HEIGHT || x < 0 || y < 0)
    return;

  uint16_t vgaChar = ((VGA_BACKGROUND << 4 | VGA_FOREGROUND) << 8) | c;
  terminalBuffer[getIndexFromPos(x, y)] = vgaChar;
}
