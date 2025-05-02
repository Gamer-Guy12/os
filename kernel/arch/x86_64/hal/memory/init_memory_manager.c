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
    region.pages = ROUND_DOWN(region.len, 4096) / 4096;
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
    new.pages = ROUND_DOWN(new.len, 4096) / 4096;
    new.type = region->type;

    region->type = phys_reserved;
    region->len = region->len - new.len;
    region->pages = ROUND_DOWN(region->len, 4096) / 4096;

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
    newPtr->pages = ROUND_DOWN(newPtr->len, 4096) / 4096;
    newPtr->prev = region;
    newPtr->next = region->next;
    region->next = newPtr;
    region->len = region->len - newPtr->len;
    region->pages = ROUND_DOWN(region->len, 4096) / 4096;

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
    newPtr[0].pages = ROUND_DOWN(newPtr[0].len, 4096) / 4096;
    newPtr[0].prev = region;
    newPtr[0].next = &newPtr[1];

    region->len = (size_t)start - (size_t)region->base;
    region->pages = ROUND_DOWN(region->len, 4096) / 4096;
    region->next = &newPtr[0];

    newPtr[1].base = end;
    newPtr[1].len = full_size - region->len - newPtr[0].len;
    newPtr[1].type = region->type;
    newPtr[1].pages = ROUND_DOWN(newPtr[1].len, 4096) / 4096;
    newPtr[1].prev = &newPtr[0];
    newPtr[1].next = next;

    // We made two regions
    section->region_count += 2;
    size_save += sizeof(phys_mem_region_t) * 2;
  }
}

/// This thing reserves space for the kernel, space that holds the physical page
/// manager, space the holds the virtual page manager, and the first mb
/// I clear all the page tables i reserve
static void create_page_tables(void) {
  PML4_entry_t *pml4 = (PML4_entry_t *)PML4_LOCATION;
  PDPT_entry_t *pdpt = (PDPT_entry_t *)BYTES_PAST(PML4_LOCATION, 0x1000);

  // Reserve space for the pml4 and the pdpt
  // We only need one pdpt which is for the last 512 gigabytes which contains
  // the last 2 gb where the kernel is
  reserve_region(get_base_section(NULL), pml4, BYTES_PAST(pml4, 0x2000));
  memset(pml4, 0, 0x2000);

  // Create the pml4 entry for the pdpt
  // Its a user page because the first 510 gb of it are not for the kernel
  pml4[511].not_executable = 0;
  pml4[511].flags = PML4_PRESENT | PML4_READ_WRITE | PDPT_USER_PAGE;

  // Set the correct bits for the addr
  pml4[511].full_entry =
      (pml4->full_entry | ((size_t)pdpt & PAGE_TABLE_ENTRY_ADDR_MASK));

  // Each entry in the pdpt takes up 1 gb
  // We need the last two which contain the higher "half"
  // These two entries contain the entire higher half so can be set to global
  // These aren't user cuz they fully contain the kernel
  pdpt[510].not_executable = 0;
  pdpt[510].flags = PDPT_PRESENT | PDPT_READ_WRITE;

  pdpt[511].not_executable = 0;
  pdpt[511].flags = PDPT_PRESENT | PDPT_READ_WRITE;
  // PDPT addresses are set below

  // This contains all the pdt entries
  // Each pdt takes holds 1 gb which means two are needed
  // 0x2000 for the two pages that the last two tables took
  PDT_entry_t *pdt = (PDT_entry_t *)BYTES_PAST(pml4, 0x2000);
  // 0x2000 For the two pages it takes
  reserve_region(get_base_section(NULL), pdt, BYTES_PAST(pdt, 0x2000));
  memset(pdt, 0, 0x2000);

  // Set pdpt addresses
  // This ors the pdpt with the address of the table it points to but masks it
  // so only the necessary parts are there
  pdpt[510].full_entry =
      pdpt[510].full_entry | ((size_t)pdt & PAGE_TABLE_ENTRY_ADDR_MASK);

  // The 0x1000 here skips over the first pdt and jumps to the second one
  pdpt[511].full_entry =
      pdpt[511].full_entry |
      ((size_t)BYTES_PAST(pdt, 0x1000) & PAGE_TABLE_ENTRY_ADDR_MASK);

  // The pml4 holds 256 tb with 512 gb per entry
  // The pdpt holds 512 gb with 1 gb per entry
  // The pdt holds 1 gb with 2 mb per entry
  // The pt holds 2 mb with 4096 kb per entry
  // By now I have put the pdpt into the pml4 so that is 512 gb
  // I then put the two pdts into the pdpt which is 2gb
  // Next is to put the pts into the pdts which is that space
  // Then point those to the pages

  // To calculate the amount of pts I need
  // The kernel length is end_kernel
  // Divide that by 0x200000 or 2mb to get the amount of pts needed
  // Then I need to add in the space to hold all the page tables themselves
  // along with the physical manager

  size_t map_length = (size_t)end_kernel;
  // This is the size of four pages for the space taken by the pml4, pdpt, and
  // the two pdts
  map_length += 0x4000;
  // Add in the amount of space taken by all the pts
  // This first calculates how many pts are needed and then the size since each
  // pt takes 0x1000 or 4kbs of space
  // This will be subtracated later after some checks and stuff
  size_t sub_later = (ROUND_UP(map_length, 0x200000) / 0x200000) * 0x1000;
  map_length += sub_later;
  map_length += (ROUND_UP(map_length, 0x200000) / 0x200000) * 0x1000;
  map_length -= sub_later;

  // Each pt can hold 0x200000 where each entry holds 0x1000
  size_t pt_count = (ROUND_UP(map_length, 0x200000) / 0x200000);

  PT_entry_t *pt = (PT_entry_t *)BYTES_PAST(pdt, 0x2000);
  reserve_region(get_base_section(NULL), pt, BYTES_PAST(pt, pt_count * 0x1000));
  memset(pt, 0, pt_count * 0x1000);

  for (size_t i = 0; i < pt_count; i++) {
    // This checks if the address is in the kernel and if it is it makes it
    // executable
    pdt[i].not_executable = i < (((size_t)end_kernel) / 0x200000) ? 0 : 1;
    // Does the same thing but checks for read write instead
    pdt[i].flags = PDT_PRESENT |
                   (i < (((size_t)end_kernel) / 0x200000) ? PDT_READ_WRITE : 0);

    uint8_t *pt_addr = (uint8_t *)pt + 4096 * i;
    pdt[i].full_entry =
        pdt[i].full_entry | ((size_t)pt_addr & PAGE_TABLE_ENTRY_ADDR_MASK);
  }

  size_t page_count = (ROUND_UP(map_length, 0x1000) / 4096);

  for (size_t i = 0; i < page_count; i++) {
    pt[i].not_executable = i < (((size_t)end_kernel) / 0x1000) ? 0 : 1;
    // Global means that this one won't be changed when page tables change since
    // this is the final page table
    pt[i].flags = PT_PRESENT | PT_GLOBAL |
                  (i < (((size_t)end_kernel) / 0x1000) ? PT_READ_WRITE : 0);
    size_t page_addr = i * 4096;
    pt[i].full_entry =
        pt[i].full_entry | ((size_t)page_addr & PAGE_TABLE_ENTRY_ADDR_MASK);
  }
}

/// This function cannot call mmap or physical map or anything cuz like they
/// depend on it being ready
///
/// This can call the physical memory manager functions after it sets up the
/// necessary prerequisites but like since the entire thing is the necessary
/// prereqs it kinda doesn't matter
///
/// But it may in the future
///
/// Actually it does cuz i can call phys alloc with the old page tables since
/// the new ones have to map it to the same spot
///
/// Wait actually no cuz the physical memory manager needs the virtual memory
/// manager which doesn't exist yet cuz of the fact that it needs to be set up
/// So you can't
///
/// This is the only function along with its descendents that are allowed to use
/// kernel_gp after that others can just use the memory manager
///
/// The PML4 the last PDPT and the two kernel PDTs are in reserved memory along
/// with all the PTs that are used
void init_memory_manager(void) {
  lock_acquire(get_mem_lock());

  // Setup size
  // Increment this size by the size of the kernel_gp that hs been used
  // It starts at 8 because the value itself is 8 bytes
  size_t *kernel_gp_64 = kernel_gp;
  kernel_gp_64[0] = 8;

  // Setup physical memory
  phys_mem_section_t base_section = create_from_multiboot();

  // Reserve kernel
  reserve_region(&base_section,
                 (void *)((uint64_t)start_kernel - KERNEL_OFFSET),
                 (void *)((uint64_t)end_kernel - KERNEL_OFFSET));
  // Reserve first mb which has like important stuff
  reserve_region(&base_section, 0x0, (void *)0x100000);

  // 0x1000 is the size of a page
  // Reserving space for the physical memory manager's first page, more can be
  // allocated and deallocated but this one must remain
  reserve_region(&base_section, kernel_gp, (uint8_t *)kernel_gp + 0x1000);

  get_base_section(&base_section);

  create_page_tables();

  lock_release(get_mem_lock());

  phys_manager_combine(get_base_section(NULL));
}
