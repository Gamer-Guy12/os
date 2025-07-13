#include <mem/pimemory.h>
#include <mem/memory.h>
#include <stddef.h>
#include <threading/pcb.h>

void create_new_addr_space(PCB_t* pcb) {
  // Create new cr3
  size_t cr3 = (size_t) phys_alloc();
  PML4_entry_t* write_cr3 = (PML4_entry_t*)(IDENTITY_MAPPED_ADDR + cr3);
  PML4_entry_t* cur_cr3 = (PML4_entry_t*)PML4_ADDR;

  write_cr3[256] = cur_cr3[256];
  write_cr3[257] = cur_cr3[257];
  write_cr3[258] = cur_cr3[258];
  write_cr3[259] = cur_cr3[259];
  write_cr3[511] = cur_cr3[511];

  write_cr3[510].full_entry = cr3;
  write_cr3[510].not_executable = 1;
  write_cr3[510].flags |= PML4_PRESENT | PML4_READ_WRITE;

  pcb->cr3 = (void*)cr3;

  vmm_kernel_region_t* region = gmalloc(sizeof(vmm_kernel_region_t));
  create_kernel_region(region);
  pcb->kernel_region = region;
}

