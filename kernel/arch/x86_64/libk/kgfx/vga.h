#ifndef VGA_LOCAL_H
#define VGA_LOCAL_H

#define VGA_WIDTH 80
#define VGA_HEIGHT 25
#define VGA_BACKGROUND 0x00
#define VGA_FOREGROUND 0x0f

#include <stdint.h>
#include <libk/kgfx.h>

static inline uint16_t getIndexFromPos(uint16_t x, uint16_t y) {
    return y * VGA_WIDTH + x;
}

#endif
