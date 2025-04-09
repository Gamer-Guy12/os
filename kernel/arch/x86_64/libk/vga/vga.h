#ifndef VGA_LOCAL_H
#define VGA_LOCAL_H

#include <stdint.h>
#include <libk/vga.h>

static inline uint16_t getIndexFromPos(uint16_t x, uint16_t y) {
    return y * VGA_WIDTH + x;
}

#endif
