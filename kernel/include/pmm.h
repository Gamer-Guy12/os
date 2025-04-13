/// This file contains the physical memory manager

#ifndef PMM_H
#define PMM_H

#include <stdint.h>

typedef uint8_t* pageframe_t;

/// By default this will allocate above 16 MB
pageframe_t allocate_page_frame(void);
/// By default this will allocate above 16 MB
pageframe_t allocate_page_frames(uint64_t count);

#endif
