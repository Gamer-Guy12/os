#ifndef X86_64_MEMORY_H
#define X86_64_MEMORY_H

#include <libk/lock.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define PAGING_NOT_EXECUTABLE 1 << 63;
#define PAGING_PRESENT 1;
#define PAGING_READ_WRITE 1 << 1;
#define PAGING_USER_MODE 1 << 2;

extern void *kernel_gp;
extern void *kernel_gp_end;
extern void *kernel_start;
extern void *end_kernel;

/// Mem lock is required to use kernel_gp for writes
/// Dont care about reads
lock_t *get_mem_lock(void);

void init_memory_manager(void);

#endif
