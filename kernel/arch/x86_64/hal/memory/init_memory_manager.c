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

  // Skip over the 8 bytes of header
  memory_map += 8;
  *used_page_counter += 1;

  multiboot_memory_t *entries = (multiboot_memory_t *)memory_map;
  size_t bytes_used = 0;

  uint64_t *qword_gp = end_kernel;

  return;
  *qword_gp = 0;
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
      entry |= 1024;

      qword_gp[bytes_used / 8] = entry;
      bytes_used += 8;
    }
  }
}

static void create_page_tables(size_t *used_page_counter) {

  kio_printf("Setup p3aging\n");
  uint64_t p4_phys_addr =
      (uint64_t)((uint64_t)end_kernel + (uint64_t)(*used_page_counter * 4096) -
                 KERNEL_OFFSET);

  (*used_page_counter) += 1;

  PML4_entry_t *p4_virt_addr =
      (PML4_entry_t *)((uint64_t)end_kernel +
                       (uint64_t)(*used_page_counter * 4096));

  // For the one page used for the l3 page tables
  (*used_page_counter) += 1;

  // Plus 0x1000 Skips over the page used by pml4
  PDPT_entry_t *p3_addr = (PDPT_entry_t *)((uint64_t)p4_virt_addr + 0x1000);

  // User page is necessary cuz this also contains non kernel pages (the entry
  // covers 512 gb but the kernel uses 2)
  p4_virt_addr[511].not_executable = 0;
  p4_virt_addr[511].flags = PML4_PRESENT | PML4_READ_WRITE | PML4_USER_PAGE;
  p4_virt_addr[511].full_entry |=
      ((uint64_t)p3_addr - KERNEL_OFFSET) & PAGE_TABLE_ENTRY_ADDR_MASK;

  // For the two pages used for the two entries in the l3 which are the 2
  // l2s that together cover the last two gb
  (*used_page_counter += 2);

  // Plus 0x1000 Skips over the page used by pdpt
  PDT_entry_t *p2_addr = (PDT_entry_t *)((uint64_t)p3_addr + 0x1000);

  p3_addr[510].not_executable = 0;
  p3_addr[510].flags = PDPT_PRESENT | PDPT_READ_WRITE;
  p3_addr[510].full_entry |=
      ((uint64_t)p2_addr - KERNEL_OFFSET) & PAGE_TABLE_ENTRY_ADDR_MASK;

  // The plus 0x1000 at the bottom skips over the first pdt
  p3_addr[511].not_executable = 0;
  p3_addr[511].flags = PDPT_PRESENT | PDPT_READ_WRITE;
  p3_addr[511].full_entry |= (((uint64_t)p2_addr + 0x1000) - KERNEL_OFFSET) &
                             PAGE_TABLE_ENTRY_ADDR_MASK;

  size_t l1s_needed =
      ROUND_UP(*used_page_counter, 0x1000 * 0x1000 * 2) / 0x1000 * 0x1000 * 2;

  (*used_page_counter) += l1s_needed;

  while (l1s_needed != ROUND_UP(*used_page_counter, 0x1000 * 0x1000 * 2) /
                           0x1000 * 0x1000 * 2) {
    (*used_page_counter) -= l1s_needed;
    l1s_needed =
        ROUND_UP(*used_page_counter, 0x1000 * 0x1000 * 2) / 0x1000 * 0x1000 * 2;
    (*used_page_counter) += l1s_needed;
  }

  // + 0x2000 Skips over the two pages used by the l2 page tables
  PT_entry_t *p1_addr = (PT_entry_t *)((uint64_t)p2_addr + 0x2000);

  for (size_t i = 0; i < l1s_needed; i++) {
    p2_addr[i].not_executable = 0;
    p2_addr[i].flags = PDT_PRESENT | PDT_READ_WRITE;
    p2_addr[i].full_entry |=
        ((uint64_t)p1_addr + 0x1000 * i) & PAGE_TABLE_ENTRY_ADDR_MASK;
  }

  // Times 512 cuz there are 512 entries in each l1
  for (size_t i = 0; i < l1s_needed * 512; i++) {
    size_t kernel_page_index_bottom =
        ((size_t)start_kernel - KERNEL_OFFSET) / 0x1000;
    size_t kernel_page_index_top =
        ((size_t)end_kernel - KERNEL_OFFSET) / 0x1000;
    p1_addr[i].not_executable =
        (i < kernel_page_index_top && i > kernel_page_index_bottom) ? 0 : 1;
    p1_addr[i].flags = PT_PRESENT | PT_GLOBAL;
    p1_addr[i].flags |=
        (i < kernel_page_index_top && i > kernel_page_index_bottom)
            ? 0
            : PT_READ_WRITE;
    p1_addr[i].full_entry |= (0x1000 * i) & PAGE_TABLE_ENTRY_ADDR_MASK;
  }

  kio_printf("Done?\n");

  // Move the address into cr3
  __asm__ __volatile__("mov %0, %%cr3" : : "r"(p4_phys_addr) : "memory");
}

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
