#ifndef _KERNEL_MEM_H_
#define _KERNEL_MEM_H_

#include "limine.h"
#include <stddef.h>
#include <stdint.h>

#ifdef _x86_64_

#define PAGE_SIZE 0x1000
#define MAX_ORDER 10
#define IDENTITY_OFFSET 0xFFFF800000000000ull
#define PAGE_STRUCT_OFFSET 0xFFFFC00000000000ull
#define BUDDY_OFFSET 0xFFFFC08000000000ull
#define KERNEL_OFFSET 0xFFFFFFFF80000000ull

#else
#error "Could not define memory constants"
#endif

#define VTP(addr) (void *)((uintptr_t)(addr) - IDENTITY_OFFSET)
#define PTV(addr) (void *)((uintptr_t)(addr) + IDENTITY_OFFSET)

void init_mem(void);
// Reads memory map and moves it to fmem
void read_memmap(void);
// Copies old page tables and creates new ones as needed
void init_page_tables(size_t entry_count, struct limine_memmap_entry **entries);

#endif
