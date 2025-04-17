#include "./x86_64_pmm.h"

static lock_t mem_lock;

lock_t *get_mem_lock(void) { return &mem_lock; }
