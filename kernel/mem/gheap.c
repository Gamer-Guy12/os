#include "kernel/kprintf.h"
#include "kernel/mem.h"
#include "kernel/gheap.h"

static struct cache size_caches[3];

void init_gheap(void) { kprintf("\t[MEM] Initialized Gheap\n"); }

void *gmalloc(size_t size, uint32_t type);

void gfree(void *ptr);
