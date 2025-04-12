#ifndef KGFX_H
#define KGFX_H

#include <stdint.h>

typedef struct  {
    uint16_t width;
    uint16_t height;
} KGFXProperties;

KGFXProperties kgfx_getproperties(void);
void kgfx_putcharat(uint16_t x, uint16_t y, char c);
void kgfx_scroll(void);
void kgfx_clear(void);

#endif
