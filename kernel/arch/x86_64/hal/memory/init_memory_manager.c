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

#define MULTIBOOT_MEMORY_MAP 6

typedef struct {
  uint64_t base;
  uint64_t len;
  uint32_t type;
  uint32_t reserved;
} __attribute__((packed)) multiboot_memory_t;

inline void *safe_fmem_push(void) {
  uint64_t ptr = (uint64_t)fmem_push();
  while (ptr > (size_t)get_multiboot() && ptr < get_multiboot_size()) {
    ptr = (uint64_t)fmem_push();
  }

  return (void *)ptr;
}

inline void safe_fmem_pop(void) {
  fmem_pop();
  while ((size_t)fmem_get_ptr(NULL) > (size_t)get_multiboot() &&
         (size_t)fmem_get_ptr(NULL) < get_multiboot_size()) {
    fmem_pop();
  }
}

static inline block_descriptor_t *create_block_descriptors(void *start,
                                                           size_t block_count) {

  block_descriptor_t *prev_ptr = NULL;
  block_descriptor_t *cur_ptr = safe_fmem_push();
  kio_printf("%u addr\n", (size_t)cur_ptr);
  return NULL;
  block_descriptor_t *first = cur_ptr;
  size_t descriptors_left =
      (PAGE_SIZE - PAGE_SIZE % sizeof(block_descriptor_t)) /
      sizeof(block_descriptor_t);

  for (size_t i = 0; i < block_count; i++) {

    cur_ptr->free_page_count = 1024;
    cur_ptr->base = (void *)((size_t)start + i * BLOCK_SIZE);

    if (i != 0) {
      cur_ptr->prev = prev_ptr;
      cur_ptr->prev->next = cur_ptr;
    }

    prev_ptr = cur_ptr;

    // Move to the next descriptor
    cur_ptr += 1;
    // If there is no space for new descriptors
    if (descriptors_left == 0) {
      cur_ptr = safe_fmem_push();
      descriptors_left = (PAGE_SIZE - PAGE_SIZE % sizeof(block_descriptor_t)) /
                         sizeof(block_descriptor_t);
    }
  }

  return first;
}

static void create_physical_map(void) {
  uint8_t *multiboot = move_to_type(MULTIBOOT_MEMORY_MAP);

  // For more info see
  // https://www.gnu.org/software/grub/manual/multiboot2/multiboot.html#Boot-information-format
  // 3.6.8
  size_t entry_count =
      (((uint32_t *)multiboot)[1] - 8) / sizeof(multiboot_memory_t);

  // Skip over static part of the header
  multiboot += 8;
  multiboot_memory_t *entries = (multiboot_memory_t *)multiboot;

  block_descriptor_t *first_descriptor = NULL;
  block_descriptor_t *last_descriptor = NULL;

  for (size_t i = 0; i < entry_count; i++) {
    if (entries[i].type != 1)
      continue;

    // A block can start at any page but must be aligned to block count
    size_t start = ROUND_UP(entries[i].base, PAGE_SIZE);
    size_t len = ROUND_DOWN(entries[i].len, BLOCK_SIZE);

    if (start > entries[i].base + entries[i].len)
      continue;

    size_t block_count = len / BLOCK_SIZE;

    if (len == 0)
      continue;

    block_descriptor_t *descriptors =
        create_block_descriptors((void *)start, block_count);

    if (first_descriptor == NULL)
      first_descriptor = descriptors;

    if (last_descriptor != NULL) {
      last_descriptor->next = descriptors;
      descriptors->prev = last_descriptor;
    }

    last_descriptor = &descriptors[block_count - 1];
  }
}

/// This function cannot call mmap or physical map or anything cuz like they
/// depend on it being ready
void init_memory_manager(void) {
  fmem_init();
  return;

  create_physical_map();

  fmem_destroy();
}
