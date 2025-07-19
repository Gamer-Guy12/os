#include <mem/pimemory.h>
#include <mem/vimemory.h>
#include <stddef.h>

static void unmap_virt_in(void* addr, void* phys_table, size_t level) {
  PT_entry_t* table = (PT_entry_t*)((size_t)phys_table + IDENTITY_MAPPED_ADDR);
  size_t index = ((size_t)addr >> (12 + level * 9)) & 0x1ff; 

  table[index].full_entry = 0;
}

// Returns addr
void* unmap_page_in(void* addr, bool free, PML4_entry_t* pml4) {
  // First get the pt, pdt, pdpt and pml4 (given) and then unmap the phys page and then unmap upwards
  size_t pml4_index = ((size_t)addr >> 12) & 0x1ff;
  void* pdpt = (void*)(pml4[pml4_index].full_entry & 0x0000fffffffff000);
  PDPT_entry_t* pdpt_entries = (PDPT_entry_t*)((size_t)pdpt + IDENTITY_MAPPED_ADDR);
  size_t pdpt_index = ((size_t)addr >> 21) & 0x1ff;
  void* pdt = (void*)(pdpt_entries[pdpt_index].full_entry & 0x0000fffffffff000);
  PDT_entry_t* pdt_entries = (PDT_entry_t*)((size_t)pdt + IDENTITY_MAPPED_ADDR);
}

