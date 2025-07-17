#ifndef X86_64_VIMEMORY_H
#define X86_64_VIMEMORY_H

#include <threading/pcb.h>
#include <libk/spinlock.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/// Gets all the physical addresses necessary
/// Returns the address u are mapping
/// If its different then there was an error
/// Does not check if page exists already
void *map_page(void *addr, uint16_t flags, bool not_executable);
/// Does not check if page exists already
void *unmap_page(void *addr, bool free);

void *map_phys_page(void *addr, uint16_t flags, bool not_executable,
                    void *phys);

void create_new_addr_space(PCB_t* pcb);

#endif
