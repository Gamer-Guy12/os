#ifndef X86_64_VIMEMORY_H
#define X86_64_VIMEMORY_H

#include <libk/bst.h>
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
void *unmap_page(void *addr);

#endif
