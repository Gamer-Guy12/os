#ifndef VGA_H
#define VGA_H

#include <stdint.h>

#define VGA_WIDTH 80
#define VGA_HEIGHT 25
#define VGA_BACKGROUND 0x00
#define VGA_FOREGROUND 0x0f

void vga_putcharat(uint16_t x, uint16_t y, char c);
void vga_scroll(void);

#endif
