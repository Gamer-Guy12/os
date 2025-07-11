#include <mem/memory.h>
#include <mem/pimemory.h>
#include <mem/vimemory.h>
#include <stdbool.h>
#include <stddef.h>

#define UNCANONICALIZER 0x0000fffffffff000

/// Checks if there are any other pages in the pt
inline static void check_pt(void *addr) {
  const size_t addr_bits = (size_t)addr;

  // Which pt in the pdt (Skip over a page for every one because each page
  // contains the contents of one pt so this contains the contents of one pdt
  // which means u need to skip)
  const size_t pdt_index = (addr_bits >> 21) & 0x1ff;
  const size_t pdpt_index = (addr_bits >> 30) & 0x1ff;
  const size_t pml4_index = (addr_bits >> 39) & 0x1ff;

  PT_entry_t *entries_to_check =
      (PT_entry_t *)(PT_ADDR + pdt_index * PAGE_SIZE +
                     pdpt_index * PAGE_SIZE * 512 +
                     pml4_index * PAGE_SIZE * 512 * 512);

  for (size_t i = 0; i < 512; i++) {
    if (entries_to_check[i].full_entry & PT_PRESENT)
      return;
  }

  unmap_virt(entries_to_check);
}

/// Checks if there are any other pages in the pdt
inline static void check_pdt(void *addr) {
  const size_t addr_bits = (size_t)addr;

  // Which pt in the pdt (Skip over a page for every one because each page
  // contains the contents of one pt so this contains the contents of one pdt
  // which means u need to skip)
  const size_t pdpt_index = (addr_bits >> 30) & 0x1ff;
  const size_t pml4_index = (addr_bits >> 39) & 0x1ff;

  PT_entry_t *entries_to_check =
      (PT_entry_t *)(PDT_ADDR + pdpt_index * PAGE_SIZE +
                     pml4_index * PAGE_SIZE * 512);

  for (size_t i = 0; i < 512; i++) {
    if (entries_to_check[i].full_entry & PDT_PRESENT)
      return;
  }

  unmap_virt(entries_to_check);
}

/// Checks if there are any other pages in the pdpt
inline static void check_pdpt(void *addr) {
  const size_t addr_bits = (size_t)addr;

  // Which pt in the pdt (Skip over a page for every one because each page
  // contains the contents of one pt so this contains the contents of one pdt
  // which means u need to skip)
  const size_t pml4_index = (addr_bits >> 39) & 0x1ff;

  PT_entry_t *entries_to_check =
      (PT_entry_t *)(PDPT_ADDR + pml4_index * PAGE_SIZE);

  for (size_t i = 0; i < 512; i++) {
    if (entries_to_check[i].full_entry & PDPT_PRESENT)
      return;
  }

  unmap_virt(entries_to_check);
}

void *unmap_page(void *addr, bool free) {
  void *phys = unmap_virt(addr);
  if (free)
    phys_free(phys);

  // Check if there are any other pages in the pt
  check_pt(addr);
  check_pdt(addr);
  check_pdpt(addr);
  // No need to check the pml4 because that will never be empty

  return addr;
}
