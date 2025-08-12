#include <mem/memory.h>
#include <mem/pimemory.h>
#include <mem/vimemory.h>
#include <stddef.h>
#include <threading/pcb.h>

void create_new_addr_space(PCB_t *pcb) {
  // Create new cr3
  size_t cr3 = (size_t)phys_alloc();
  PML4_entry_t *write_cr3 = (PML4_entry_t *)(IDENTITY_MAPPED_ADDR + cr3);
  PML4_entry_t *cur_cr3 = (PML4_entry_t *)PML4_ADDR;

  for (size_t i = 256; i < 512; i++) {
    if (i != 510)
      write_cr3[i] = cur_cr3[i];
  }


  write_cr3[510].full_entry = cr3;
  write_cr3[510].not_executable = 1;
  write_cr3[510].flags |= PML4_PRESENT | PML4_READ_WRITE;

  pcb->cr3 = (void *)cr3;
}

// typedef enum {
//   LEVEL_PML4 = 4,
//   LEVEL_PDPT = 3,
//   LEVEL_PDT = 2,
//   LEVEL_PT = 1,
//   LEVEL_PAGE = 0
// } page_levels_t;
// 
// void unmap_level(size_t phys_addr, page_levels_t level) {
//   if (level == LEVEL_PAGE) {
//     phys_free((void *)phys_addr);
// 
//     return;
//   }
// 
//   PT_entry_t *entries = (PT_entry_t *)(phys_addr + IDENTITY_MAPPED_ADDR);
// 
//   for (size_t i = 0; i < 512; i++) {
//     size_t addr = entries->full_entry & 0x0000fffffffff000;
//     unmap_level(addr, level - 1);
//   }
// 
//   phys_free((void *)phys_addr);
// }

