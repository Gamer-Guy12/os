#include <libk/kio.h>
#include <libk/kgfx.h>

static uint16_t curX = 0;
static uint16_t curY = 0;
static uint16_t width = 0;
static uint16_t height = 0;

void kio_putchar(char c) {
    KGFXProperties props = kgfx_getproperties();
    width = props.width;
    height = props.height;

    if (c == '\n') {
        curX = 0;
        curY++;
        return;
    } else if (c == '\r') {
        curX = 0;
        return;
    } else if (c == '\b') {
        if (curX == 0) {
            return;
        }

        curX--;
        return;
    } else if (c == '\t') {
        if (curX >= width - 1) {
            return;
        }

        curX++;
        return;
    }

    if (curX >= width) {
        curY++;
        curX = 0;
    }

    if (curY >= height) {
        kgfx_scroll();
        curY--;
        curX = 0;
    }

    kgfx_putcharat(curX, curY, c);
    curX++;
}
