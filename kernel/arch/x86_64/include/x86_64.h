#ifndef X86_64_KERNEL_H
#define X86_64_KERNEL_H

#include <stddef.h>
#include <stdint.h>

#define KERNEL_OFFSET 0xFFFF800000000000
#define KERNEL_CODE_OFFSET 0xFFFFFFFF80000000
#define PAGE_TABLE_OFFSET 0x8000000000
#define MEMORY_MANAGEMENT_OFFSET 0x10000000000

#define PAGE_INDICES_TO_ADDR(pml4, pdpt, pdt, pt, addr)                        \
  ((addr & 0xfff) | ((pt & 0x1ff) << 12) | ((pdt & 0x1ff) << 21) |             \
   ((pdpt & 0x1ff) << 30) | ((pml4 & 0x1ff) << 39))

void start_cores(void);

void init_multiboot(uint8_t *multiboot);
uint8_t *get_multiboot(void);
size_t get_multiboot_size(void);

uint8_t *move_to_type(uint32_t type);

#define IA32_EFER 0xC0000080

#endif
