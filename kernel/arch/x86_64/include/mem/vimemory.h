#ifndef X86_64_VIMEMORY_H
#define X86_64_VIMEMORY_H

#include <libk/spinlock.h>
#include <mem/pimemory.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <threading/pcb.h>

/// Gets all the physical addresses necessary
/// Returns the address u are mapping
/// If its different then there was an error
/// Does not check if page exists already
void *map_page(void *addr, uint16_t flags, bool not_executable);
void *map_page_in(void *addr, uint16_t flags, bool not_executable,
                  PML4_entry_t *pml4);
/// Does not check if page exists already
void *unmap_page(void *addr, bool free);
void *unmap_page_in(void *addr, bool free, PML4_entry_t *pml4);

void *map_phys_page(void *addr, uint16_t flags, bool not_executable,
                    void *phys);

void create_new_addr_space(PCB_t *pcb);

spinlock_t *WUNUSED get_table_lock(void);

#endif
