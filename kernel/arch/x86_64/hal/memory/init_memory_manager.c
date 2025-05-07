#include "libk/kio.h"
#include <hal/imemory.h>
#include <hal/memory.h>
#include <libk/lock.h>
#include <libk/math.h>
#include <libk/mem.h>
#include <libk/sys.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <x86_64.h>

typedef struct {
  uint64_t base;
  uint64_t len;
  uint32_t type;
  uint32_t reserved;
} __attribute__((packed)) multiboot_memory_t;

void create_physical_map(size_t *used_page_counter) {
  uint8_t *memory_map = move_to_type(6);
  size_t entry_count =
      (((uint32_t *)memory_map)[1] - 8) / ((uint32_t *)memory_map)[2];

  // Skip over the memory map header
  memory_map += 8;
}

static void create_page_tables(size_t *used_page_counter) {}

static void reserve_used_chunks(size_t *used_page_counter) {}

/// This function cannot call mmap or physical map or anything cuz like they
/// depend on it being ready
void init_memory_manager(void) {
  lock_acquire(get_mem_lock());

  size_t used_page_counter = 0;

  create_physical_map(&used_page_counter);
  kio_printf("Im fine\n");

  create_page_tables(&used_page_counter);

  reserve_used_chunks(&used_page_counter);

  lock_release(get_mem_lock());
}
