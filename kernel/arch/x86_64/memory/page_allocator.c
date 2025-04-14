#include "./x86_64_pmm.h"
#include <pmm.h>
#include <stddef.h>

typedef struct {
    uint8_t* start;
    size_t length;
    size_t pages_used;
} used_mem_region_t;

pageframe_t allocate_page_frame(void) {
    return 0;
}
