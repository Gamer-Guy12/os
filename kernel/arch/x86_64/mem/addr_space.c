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

  write_cr3[256] = cur_cr3[256];
  write_cr3[257] = cur_cr3[257];
  write_cr3[258] = cur_cr3[258];
  write_cr3[259] = cur_cr3[259];
  write_cr3[511] = cur_cr3[511];

  write_cr3[510].full_entry = cr3;
  write_cr3[510].not_executable = 1;
  write_cr3[510].flags |= PML4_PRESENT | PML4_READ_WRITE;

  pcb->cr3 = (void *)cr3;

  vmm_kernel_region_t *region = gmalloc(sizeof(vmm_kernel_region_t));
  create_kernel_region(region);
  pcb->kernel_region = region;
}

typedef enum {
  LEVEL_PML4 = 4,
  LEVEL_PDPT = 3,
  LEVEL_PDT = 2,
  LEVEL_PT = 1,
  LEVEL_PAGE = 0
} page_levels_t;

void unmap_level(size_t phys_addr, page_levels_t level) {
  if (level == LEVEL_PAGE) {
    phys_free((void *)phys_addr);

    return;
  }

  PT_entry_t *entries = (PT_entry_t *)(phys_addr + IDENTITY_MAPPED_ADDR);

  for (size_t i = 0; i < 512; i++) {
    size_t addr = entries->full_entry & 0x0000fffffffff000;
    unmap_level(addr, level - 1);
  }

  phys_free((void *)phys_addr);
}

void delete_addr_space(PCB_t *pcb) {
  // First delete the kernel and user region (if they exist) and then unmap all
  // the pages

  if (pcb->kernel_region)
    gfree(pcb->kernel_region);
  if (pcb->user_region)
    gfree(pcb->user_region);

  // Unmap everything
  unmap_level((size_t)pcb->cr3, LEVEL_PML4);
}
