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

/// Actually unsafe cuz ofl ike things where it pops and pushes without a lock
inline void *safe_fmem_multi_push(size_t count) {
  uint64_t ptr = (uint64_t)fmem_push();
  while (ptr > (size_t)get_multiboot() && ptr < get_multiboot_size()) {
    ptr = (uint64_t)fmem_push();
  }
  fmem_pop();
  fmem_multi_push(count);

  return (void *)ptr;
}

static inline block_descriptor_t *create_block_descriptors(void *start,
                                                           size_t block_count) {

  block_descriptor_t *prev_ptr = NULL;
  block_descriptor_t *cur_ptr = safe_fmem_push();
  block_descriptor_t *first = cur_ptr;
  size_t descriptors_left = ROUND_DOWN(PAGE_SIZE, sizeof(block_descriptor_t)) /
                            sizeof(block_descriptor_t);

  for (size_t i = 0; i < block_count; i++) {

    // kio_printf("Done %u, %u\n", i, block_count);

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
      descriptors_left = ROUND_DOWN(PAGE_SIZE, sizeof(block_descriptor_t)) /
                         sizeof(block_descriptor_t);
    }
  }

  return first;
}

static void create_physical_map(void) {
  uint8_t *multiboot = move_to_type(MULTIBOOT_MEMORY_MAP);

  if (multiboot == NULL)
    sys_panic(3);

  // For more info see
  // https://www.gnu.org/software/grub/manual/multiboot2/multiboot.html#Boot-information-format
  // 3.6.8
  size_t entry_count =
      (((uint32_t *)multiboot)[1] - 16) / sizeof(multiboot_memory_t);

  // Skip over static part of the header
  multiboot += 16;
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

    if (len > entries[i].len || entries[i].len < BLOCK_SIZE)
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

static void create_page_tables(void) {

  PML4_entry_t *pml4 = safe_fmem_push();
  memset(pml4, 0, PAGE_SIZE);

  PDPT_entry_t *pdpt = safe_fmem_push();
  memset(pdpt, 0, PAGE_SIZE);

  pml4[511].flags = PML4_USER_PAGE | PML4_READ_WRITE | PML4_PRESENT;
  pml4[511].not_executable = 0;
  pml4[511].full_entry |= PAGE_ADDR(pdpt);

  // Save 2 pages (hopefully contigouous)
  // Should be cuz no other concurrent things
  PDT_entry_t *pdt = safe_fmem_push();
  safe_fmem_push();
  memset(pdt, 0, PAGE_SIZE * 2);

  pdpt[510].flags = PDPT_PRESENT | PDPT_READ_WRITE;
  pdpt[510].not_executable = 0;
  pdpt[510].full_entry |= PAGE_ADDR(pdt);

  pdpt[511].flags = PDPT_PRESENT | PDPT_READ_WRITE;
  pdpt[511].not_executable = 0;
  pdpt[511].full_entry |= PAGE_ADDR(&pdt[512]);

#define MB2 0x200000

  size_t mb2s_needed = ROUND_UP((size_t)fmem_get_ptr(NULL), MB2) / MB2 + 1;

  extern char _text_end[];
  const void *text_end = _text_end;

  for (size_t i = 0; i < mb2s_needed; i++) {
    PT_entry_t *pt = safe_fmem_push();
    memset(pt, 0, PAGE_SIZE);

    pdt[i].flags = PDT_PRESENT | PDT_READ_WRITE;
    pdt[i].not_executable = 0;
    pdt[i].full_entry |= PAGE_ADDR(pt);

    // 512 entries in a pt
    for (size_t j = 0; j < 512; j++) {
      size_t cur_addr = MB2 * i + PAGE_SIZE * j;

      pt[i].flags = PT_PRESENT;
      if (cur_addr > (size_t)text_end - KERNEL_OFFSET) {
        pt[i].flags |= PT_READ_WRITE;
        pt[i].not_executable = 1;
      } else {
        pt[i].not_executable = 0;
      }
      pt[i].full_entry |= PAGE_ADDR(cur_addr);
    }
  }

  __asm__ volatile("movq %0, %%cr3"
                   :
                   : "r"((size_t)pml4 - KERNEL_OFFSET)
                   : "memory");
}

/// This function cannot call mmap or physical map or anything cuz like they
/// depend on it being ready
void init_memory_manager(void) {
  fmem_init();

  create_physical_map();

  create_page_tables();

  fmem_destroy();
}
