#ifndef _KERNEL_MEM_H_
#define _KERNEL_MEM_H_

#include <stdint.h>

#ifdef _x86_64_

#define PAGE_SIZE 0x1000
#define MAX_ORDER 10
#define IDENTITY_OFFSET 0xFFFF800000000000

#else
#error "Could not define memory constants"
#endif

#define VTP(addr) (void *)((uintptr_t)(addr) - IDENTITY_OFFSET)
#define PTV(addr) (void *)((uintptr_t)(addr) + IDENTITY_OFFSET)

// Reads memory map and moves it to fmem
void read_memmap(void);
void init_mem(void);

#endif

