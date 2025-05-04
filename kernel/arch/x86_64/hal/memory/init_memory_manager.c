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

static void create_physical_map(size_t *used_page_counter) {
  uint8_t *memory_map = move_to_type(6);

  size_t entry_count =
      (((uint32_t *)memory_map)[1] - 8) / ((uint32_t *)memory_map)[2];

  // Skip over the 8 bytes of header
  memory_map += 8;
  *used_page_counter += 1;

  multiboot_memory_t *entries = (multiboot_memory_t *)memory_map;
  size_t bytes_used = 0;

  uint64_t *qword_gp = kernel_gp;

  qword_gp[0] = (uint64_t)NULL;
  bytes_used += 8;

  for (size_t i = 0; i < entry_count; i++) {
    if (entries[i].type != 1)
      continue;

    uint64_t aligned_base = ROUND_UP(entries[i].base, 0x1000 * 0x1000 * 0x4);
    if (aligned_base > entries[i].base + entries[i].len)
      continue;

    uint64_t aligned_len = ROUND_DOWN(entries[i].len, 0x1000 * 0x1000 * 0x4);

    if (aligned_len == 0)
      continue;

    size_t chunks = aligned_len / (0x1000 * 0x1000 * 0x4);

    for (size_t j = 0; j < chunks; j++) {
      if (bytes_used % 4096 == 0) {
        // Get another page
        (*used_page_counter) += 1;

        qword_gp[bytes_used - 4096] = (size_t)(&(qword_gp[bytes_used]));
      }

      uint64_t entry = 0;
      entry |= entries[i].base + j * 0x1000 * 0x1000 * 0x4;
      // This means non-reserved
      entry |= 0 << 21;
      entry |= 4096;

      qword_gp[bytes_used / 8] = entry;
      bytes_used += 8;
    }
  }
}

static void create_page_tables(size_t *used_page_counter) {}

static void reserve_used_chunks(size_t *used_page_counter) {}

/// This function cannot call mmap or physical map or anything cuz like they
/// depend on it being ready
void init_memory_manager(void) {
  lock_acquire(get_mem_lock());

  size_t used_page_counter = 0;

  create_physical_map(&used_page_counter);

  create_page_tables(&used_page_counter);

  reserve_used_chunks(&used_page_counter);

  lock_release(get_mem_lock());
}
