/// This file contains the physical memory manager

#ifndef PMM_H
#define PMM_H

#include <stdint.h>

typedef uint8_t *pageframe_t;

void setup_page_frame_allocation(uint8_t *multiboot);
/// By default this will allocate above 16 MB
pageframe_t allocate_page_frame(void);
void free_page_frame(pageframe_t *page);

#endif
