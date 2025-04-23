/// This file contains the physical memory manager

#ifndef PMM_H
#define PMM_H

#include <stdint.h>

typedef uint8_t *pageframe_t;

void setup_page_frame_allocation(uint8_t *multiboot);
pageframe_t allocate_page_frame(void);
void allocate_page_frames(pageframe_t *pages, uint64_t count);
void free_page_frame(pageframe_t *page);

#endif
