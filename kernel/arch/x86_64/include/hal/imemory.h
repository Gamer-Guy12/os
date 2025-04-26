#ifndef X86_64_MEMORY_H
#define X86_64_MEMORY_H

#include <libk/lock.h>
#include <stdbool.h>
#include <stddef.h>

extern void *kernel_gp;
extern void *kernel_gp_end;

lock_t *get_mem_lock(void);

void *phys_map(size_t page_count, bool contiguous);
void phys_unmap(void *page, size_t page_count);

void init_memory_manager(uint8_t *multiboot);

typedef enum {
  phys_reserved = 0,
  phys_free = 1,
  phys_used = 2
} phys_mem_region_type_t;

typedef struct {
  void *start_addr;
  size_t length;
  phys_mem_region_type_t type;
} phys_mem_region_t;

#endif
