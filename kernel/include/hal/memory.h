#ifndef MEMORY_H
#define MEMORY_H

#include <stdbool.h>
#include <stddef.h>

typedef enum { phys_reserved, phys_free, phys_allocated } phys_mem_type_t;

struct phys_mem_region_struct {
  void *base;
  size_t len;
  size_t pages;
  struct phys_mem_region_struct *next;
  struct phys_mem_region_struct *prev;
  phys_mem_type_t type;
};

typedef struct phys_mem_region_struct phys_mem_region_t;

typedef struct {
  void *base;
  size_t size;
  size_t region_count;
  phys_mem_region_t *first;
} phys_mem_section_t;

void *phys_alloc(phys_mem_section_t *section);
/// These are contiguous pages, just use phys_alloc multiple times to get
/// multiple non-contiguous ones
void *phys_multi_alloc(phys_mem_section_t *section, size_t page_count);
void phys_reserve(phys_mem_section_t *section, size_t page_offset,
                  size_t page_count);
void phys_unreserve(phys_mem_region_t *region, phys_mem_section_t *section);

#endif
