#ifndef X86_64_MEMORY_H
#define X86_64_MEMORY_H

#include <libk/lock.h>
#include <stdbool.h>
#include <stddef.h>

#define PAGING_NOT_EXECUTABLE 1 << 63;
#define PAGING_PRESENT 1;
#define PAGING_READ_WRITE 1 << 1;

extern void *kernel_gp;
extern void *kernel_gp_end;

lock_t *get_mem_lock(void);

void *phys_map(void);
void *phys_map_contig(size_t page_count);
void phys_unmap(void *page, size_t page_count);

void init_memory_manager(void);

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
