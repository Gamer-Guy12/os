#include <x86_64.h>

static uint8_t *global_multiboot;

void init_multiboot(uint8_t *multiboot) { global_multiboot = multiboot; }

uint8_t *get_multiboot(void) { return global_multiboot; }
