#include <libk/vga.h>
#include "./vga.h"

static volatile uint16_t* terminalBuffer = (volatile uint16_t*) (0xB8000);

void vga_scroll(void) {
    for (uint16_t i = 0; i < VGA_HEIGHT; i++) {
        if (i == 0) continue;

        for (uint16_t j = 0; j < VGA_WIDTH; j++) {
            uint16_t curInd = getIndexFromPos(j, i);

            uint16_t prevLineInd = curInd - VGA_WIDTH;

            terminalBuffer[prevLineInd] = terminalBuffer[curInd];

            if (i == VGA_HEIGHT - 1) {
                terminalBuffer[curInd] = 0x00;
            }
        }
    }
}
