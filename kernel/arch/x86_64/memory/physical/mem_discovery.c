/// Here the memory map is discovered

#include "x86_64_pmm.h"
#include <pmm.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

static bool check_for_type(uint8_t *multiboot, uint32_t desiredType) {
  uint32_t type = *((uint32_t *)multiboot);

  return type == desiredType;
}

uint8_t *move_to_memory_map(uint8_t *multiboot) {
  // Increment the pointer to skip over until ou get to the start of the memory
  // map
  multiboot += 8;
  while (!check_for_type(multiboot, 6)) {
    uint32_t size = ((uint32_t *)multiboot)[1];
    if (size % 8 != 0)
      size += (8 - (size % 8));

    multiboot += size;
  }

  return multiboot;
}

memory_map_t get_memory_map(uint8_t *multiboot) {
  memory_map_t map;
  uint8_t *ptr = move_to_memory_map(multiboot);
  // The plus 16 skips over the sixteen bytes of metadata and goes straight to
  // the entries
  map.ptr = (memory_map_entry_t *)(ptr + 16);
  uint32_t size = ((uint32_t *)ptr)[1];
  uint32_t entry_size = ((uint32_t *)ptr)[2];
  /// This gets rid of the metadata
  size -= 16;
  map.entryCount = size / entry_size;

  return map;
}

uint64_t get_memory_size(memory_map_t map) {
  uint64_t size = 0;

  for (uint64_t i = 0; i < map.entryCount; i++) {
    size += map.ptr[i].length;
  }

  return size;
}
