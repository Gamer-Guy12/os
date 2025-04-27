#include "x86_64.h"
#include <hal/imemory.h>
#include <libk/lock.h>
#include <libk/mem.h>
#include <stddef.h>
#include <stdint.h>

typedef struct {
  uint64_t base_addr;
  uint64_t length;
  uint32_t type;
  uint32_t reserved;
} multiboot_memory_entry_t;

static uint32_t get_entry_count(uint8_t *memory_map) {
  uint32_t size = memory_map[4] | (memory_map[5] << 8) | (memory_map[6] << 16) |
                  (memory_map[7] << 24);

  // Substract away the beginning 16 bytes that aren't entries
  size -= 16;

  // Each entry is 24 bytes long so divide by that to get the full size
  return size / 24;
}

/// This function cannot call mmap or physical map or anything cuz like they
/// depend on it being ready
void init_memory_manager(void) {
  lock_acquire(get_mem_lock());

  uint8_t *memory_map = move_to_type(6);
  uint32_t entry_count = get_entry_count(memory_map);

  // Skip over the beginning 16 bytes to get to the entries
  multiboot_memory_entry_t *entries =
      (multiboot_memory_entry_t *)(memory_map + 16);

  phys_mem_region_t phys_regions[entry_count];

  for (size_t i = 0; i < entry_count; i++) {
    phys_regions[i].start_addr = (void *)entries[i].base_addr;
    phys_regions[i].length = entries[i].length;
    if (entries[i].type == 1) {
      phys_regions[i].type = phys_free;
    } else {
      phys_regions[i].type = phys_reserved;
    }
  }

  // Move the physical memory regions into the general purpose memory to be
  // mapped into virtual memory
  memcpy(phys_regions, kernel_gp, entry_count * sizeof(phys_mem_region_t));

  lock_release(get_mem_lock());
}
