#ifndef X86_64_KERNEL_H
#define X86_64_KERNEL_H

#include <stdint.h>

void start_cores(void);

void init_multiboot(uint8_t *multiboot);
uint8_t *get_multiboot(void);

uint8_t *move_to_type(uint32_t type);

#endif
