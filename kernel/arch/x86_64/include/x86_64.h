#ifndef X86_64_KERNEL_H
#define X86_64_KERNEL_H

#include <stdint.h>

#define KERNEL_OFFSET 0xFFFFFFFF80000000
#define KERNEL_CODE_OFFSET 0x100000
#define PAGE_TABLE_OFFSET 0x8000000000
#define MEMORY_MANAGEMENT_OFFSET 0x10000000000

void start_cores(void);

void init_multiboot(uint8_t *multiboot);
uint8_t *get_multiboot(void);

uint8_t *move_to_type(uint32_t type);

#endif
