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

  for (size_t i = 0; i < entry_count; i++) {
  }
}

/// This function cannot call mmap or physical map or anything cuz like they
/// depend on it being ready
void init_memory_manager(void) {
  fmem_init();

  create_physical_map();

  fmem_destroy();
}
