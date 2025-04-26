#ifndef MEMORY_H
#define MEMORY_H

#include <stdbool.h>
#include <stddef.h>

/// Void maps a certain amount of pages into virtual memory
/// They are all contiguous in virtual memory
/// @param contiguous Defines whether they should be contiguous in physical
/// memory
void *hal_mmap(size_t page_count, bool contiguous);

/// Unmaps a certain amount of contiguous virtual pages from memory
/// These are both kernel only versions that will then be used by system calls
/// and should not be used by usermode
void hal_munmap(void *page, size_t page_count);

/// Reserves a physical page so that it can't be allocated or deallocated
void res_phys_page(void *phys_addr);

/// Reserves a virtual page so that it can't be mapped or unmapped
void res_virt_page(void *virt_addr);

#endif
