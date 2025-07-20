#include <libk/spinlock.h>
#include <mem/memory.h>
#include <mem/pimemory.h>
#include <mem/vimemory.h>
#include <stddef.h>

// Returns addr
void *unmap_page_in(void *addr, bool free, PML4_entry_t *pml4) {
  spinlock_acquire(get_table_lock());

  // First get the pt, pdt, pdpt and pml4 (given) and then unmap the phys page
  // and then unmap upwards
  size_t pml4_index = ((size_t)addr >> 39) & 0x1ff;
  void *pdpt = (void *)(pml4[pml4_index].full_entry & 0x0000fffffffff000);
  PDPT_entry_t *pdpt_entries =
      (PDPT_entry_t *)((size_t)pdpt + IDENTITY_MAPPED_ADDR);
  size_t pdpt_index = ((size_t)addr >> 30) & 0x1ff;
  void *pdt =
      (void *)(pdpt_entries[pdpt_index].full_entry & 0x0000fffffffff000);
  PDT_entry_t *pdt_entries =
      (PDT_entry_t *)((size_t)pdt + IDENTITY_MAPPED_ADDR);
  size_t pdt_index = ((size_t)addr >> 21) & 0x1ff;
  void *pt = (void *)(pdt_entries[pdt_index].full_entry & 0x0000fffffffff000);
  PT_entry_t *pt_entries = (PT_entry_t *)((size_t)pt + IDENTITY_MAPPED_ADDR);
  size_t pt_index = ((size_t)addr >> 12) & 0x1ff;

  // Unmap phys page
  size_t phys_addr = pt_entries[pt_index].full_entry & 0x0000fffffffff000;
  if (free) {
    phys_free((void *)phys_addr);
  }
  pt_entries[pt_index].full_entry = 0;

  // Check if there is anything else in the PT
  bool unmap_pt = true;
  for (size_t i = 0; i < 512; i++) {
    if (pt_entries[i].full_entry != 0) {
      unmap_pt = false;
      break;
    }
  }

  if (unmap_pt) {
    size_t phys_addr = pdt_entries[pdt_index].full_entry & 0x0000fffffffff000;
    phys_free((void *)phys_addr);
    pdt_entries[pdt_index].full_entry = 0;
  }

  // Do the same for unmapping the pdt
  bool unmap_pdt = true;
  for (size_t i = 0; i < 512; i++) {
    if (pdt_entries[i].full_entry != 0) {
      unmap_pdt = false;
      break;
    }
  }

  if (unmap_pdt) {
    size_t phys_addr = pdpt_entries[pdpt_index].full_entry & 0x0000fffffffff000;
    phys_free((void *)phys_addr);
    pdpt_entries[pdpt_index].full_entry = 0;
  }

  // Finally unmap the pdpt
  bool unmap_pdpt = false;
  for (size_t i = 0; i < 512; i++) {
    if (pdpt_entries[i].full_entry != 0) {
      unmap_pdpt = false;
      break;
    }
  }

  if (unmap_pdpt) {
    size_t phys_addr = pml4[pml4_index].full_entry & 0x0000fffffffff000;
    phys_free((void *)phys_addr);
    pml4[pml4_index].full_entry = 0;
  }

  spinlock_release(get_table_lock());

  return addr;
}
