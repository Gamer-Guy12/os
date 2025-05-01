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
  uint64_t base_addr;
  uint64_t length;
  uint32_t type;
  uint32_t reserved;
} __attribute__((packed)) multiboot_memory_entry_t;

static uint32_t get_entry_count(uint8_t *memory_map) {
  uint32_t size = memory_map[4] | (memory_map[5] << 8) | (memory_map[6] << 16) |
                  (memory_map[7] << 24);

  // Substract away the beginning 16 bytes that aren't entries
  size -= 16;

  // Each entry is 24 bytes long so divide by that to get the full size
  return size / 24;
}

/// Creates the memory regions and base section from multiboot
/// Copies the regions into the start of kernel_gp
/// Space for these should later then be allocated themselves into as many pages
/// as necessary
static phys_mem_section_t create_from_multiboot(void) {
  uint8_t *memory_map = move_to_type(6);

  uint32_t entry_count = get_entry_count(memory_map);

  // Skip over the header
  memory_map += 16;
  multiboot_memory_entry_t *entries = (multiboot_memory_entry_t *)memory_map;
  // The + 8 skips over the 8 bytes used to save the size used
  phys_mem_region_t *save_space = (void *)((uint8_t *)kernel_gp + 8);
  size_t *size_save = kernel_gp;
  size_t len = 0;

  for (uint32_t i = 0; i < entry_count; i++) {
    phys_mem_region_t region;
    region.base = (void *)entries[i].base_addr;
    region.len = entries[i].length;
    region.pages = ROUND_DOWN(region.len / 4096, 4096);
    region.type = entries[i].type == 1 ? phys_free : phys_allocated;

    if (i > 0)
      region.prev = &(save_space[i - 1]);

    if (i < entry_count - 1)
      region.next = &(save_space[i + 1]);

    save_space[i] = region;

    len += region.len;
    size_save += sizeof(region);
  }

  phys_mem_section_t base_sect;
  // This will be overwritten but just stop vscode and compiler complaining
  // about unused variable
  base_sect.size = *size_save;
  base_sect.first = save_space;
  base_sect.region_count = entry_count;
  base_sect.size = len;
  base_sect.base = (void *)entries[0].base_addr;

  return base_sect;
}

/// The entire reserved part must be in one region currently
/// That means that u have to free the entire region and it has to be not in a
/// reserved space already
/// End is the byte after the end of the region
static void reserve_region(phys_mem_section_t *section, void *start,
                           void *end) {
  phys_mem_region_t *region = NULL;
  phys_mem_region_t *cur_region = section->first;

  for (size_t i = 0; i < section->region_count; i++) {
    if (cur_region->base > start &&
        cur_region->len > (size_t)end - (size_t)start) {
      region = cur_region;
      break;
    }

    cur_region = cur_region->next;
  }

  if (region == NULL)
    sys_panic(0);

  if (!(((size_t)end - (size_t)start) > region->len))
    sys_panic(0);

  if (region->type == phys_reserved)
    return;

  // This long assignment basically skips over the used part of it
  size_t *size_save = kernel_gp;

  // True means it does what the name says
  bool start_equal = start == region->base;
  bool end_equal = (size_t)end == (size_t)region->base + region->len;

  // Easiest case
  if (start_equal && end_equal) {
    region->type = phys_reserved;
  } else if (start_equal && !end_equal) {
    phys_mem_region_t new;
    new.base = end;
    new.len = region->len - ((size_t)end - (size_t)start);
    new.pages = ROUND_DOWN(new.len / 4096, 4096);
    new.type = region->type;

    region->type = phys_reserved;
    region->len = region->len - new.len;
    region->pages = ROUND_DOWN(region->len / 4096, 4096);

    phys_mem_region_t *newPtr =
        &((phys_mem_region_t *)((uint8_t *)kernel_gp +
                                8))[*size_save / sizeof(phys_mem_region_t)];
    *newPtr = new;

    newPtr->next = region->next;
    region->next = newPtr;
    newPtr->prev = region;

    // Remember to increment section count
    section->region_count++;
    size_save += sizeof(phys_mem_region_t);
  } else if (!start_equal && end_equal) {
    // Region is the first part and normal
    // New is the second and reserved

    phys_mem_region_t *newPtr =
        &((phys_mem_region_t *)((uint8_t *)kernel_gp +
                                8))[*size_save / sizeof(phys_mem_region_t)];

    newPtr->base = start;
    newPtr->len = (size_t)end - (size_t)start;
    newPtr->type = phys_reserved;
    newPtr->pages = ROUND_DOWN(newPtr->len / 4096, 4096);
    newPtr->prev = region;
    newPtr->next = region->next;
    region->next = newPtr;
    region->len = region->len - newPtr->len;
    region->pages = ROUND_DOWN(region->len / 4096, 4096);

    section->region_count++;
    size_save += sizeof(phys_mem_region_t);
  } else {
    // Region is first and stays the same
    // newPtr[0] is second and represents the reserved
    // newPtr[1] is third and stays the same as region

    phys_mem_region_t *newPtr =
        &((phys_mem_region_t *)((uint8_t *)kernel_gp +
                                8))[*size_save / sizeof(phys_mem_region_t)];

    size_t full_size = region->len;
    phys_mem_region_t *next = region->next;

    newPtr[0].base = start;
    newPtr[0].len = (size_t)end - (size_t)start;
    newPtr[0].type = phys_reserved;
    newPtr[0].pages = ROUND_DOWN(newPtr[0].len / 4096, 4096);
    newPtr[0].prev = region;
    newPtr[0].next = &newPtr[1];

    region->len = (size_t)start - (size_t)region->base;
    region->pages = ROUND_DOWN(region->len / 4096, 4096);
    region->next = &newPtr[0];

    newPtr[1].base = end;
    newPtr[1].len = full_size - region->len - newPtr[0].len;
    newPtr[1].type = region->type;
    newPtr[1].pages = ROUND_DOWN(newPtr[1].len / 4096, 4096);
    newPtr[1].prev = &newPtr[0];
    newPtr[1].next = next;

    // We made two regions
    section->region_count += 2;
    size_save += sizeof(phys_mem_region_t) * 2;
  }
}

static void create_page_tables(void) {}

/// This function cannot call mmap or physical map or anything cuz like they
/// depend on it being ready
void init_memory_manager(void) {
  lock_acquire(get_mem_lock());

  // Setup size
  // Increment this size by the size of the kernel_gp that hs been used
  size_t *kernel_gp_64 = kernel_gp;
  kernel_gp_64[0] = 0;

  // Setup physical memory
  phys_mem_section_t base_section = create_from_multiboot();

  // Reserve kernel
  reserve_region(&base_section,
                 (void *)((uint64_t)start_kernel - KERNEL_OFFSET),
                 (void *)((uint64_t)end_kernel - KERNEL_OFFSET));
  // Reserve first mb which has like important stuff
  reserve_region(&base_section, 0x0, (void *)0x100000);

  get_base_section(base_section);

  create_page_tables();

  lock_release(get_mem_lock());
}
