#include <libk/kio.h>
#include <libk/vga.h>

static uint16_t curX = 0;
static uint16_t curY = 0;

void kio_putchar(char c) {
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
        if (curX >= VGA_WIDTH - 1) {
            return;
        }

        curX++;
        return;
    }

    if (curX >= VGA_WIDTH) {
        curY++;
        curX = 0;
    }

    if (curY >= VGA_HEIGHT) {
        vga_scroll();
        curY--;
        curX = 0;
    }

    vga_putcharat(curX, curY, c);
    curX++;
}
