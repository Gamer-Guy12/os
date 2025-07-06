#include <cls.h>
#include <gdt.h>
#include <hal/hal.h>
#include <interrupts.h>
#include <libk/kio.h>
#include <libk/vga_kgfx.h>
#include <mem/kheap.h>
#include <mem/memory.h>
#include <mem/pimemory.h>
#include <multiboot.h>
#include <stddef.h>
#include <stdint.h>
#include <x86_64.h>

void setup_memory(void) {
  PML4_entry_t *pml4 =
      (PML4_entry_t *)((size_t)phys_alloc() + IDENTITY_MAPPED_ADDR);
  PML4_entry_t *cur_pml4 = (PML4_entry_t *)(PML4_ADDR);

  // Copy 256 (Physical structures)
  pml4[256] = cur_pml4[256];

  // Copy over 257 (Identity mapping)
  pml4[257] = cur_pml4[257];

  // Copy over 258 (Misc information)
  pml4[258] = cur_pml4[258];

  // Recursive mapping
  size_t phys_pml4 = (size_t)pml4 - IDENTITY_MAPPED_ADDR;
  pml4[510].full_entry = phys_pml4;
  pml4[510].flags |= PT_PRESENT | PT_READ_WRITE;
  pml4[510].not_executable = 1;

  // Copy over 511 (Kernel)
  pml4[511] = cur_pml4[511];

  // Move it into cr3
  __asm__ volatile("mov %%rax, %%cr3" ::"a"(phys_pml4) : "memory");

  /// TODO: Delete the old stacks and create new ones
}

void smp_start(uint32_t processor_id) {
  kio_printf("0 Hi 0\n");
  setup_memory();
  kio_printf("Hi 1\n");

  vmm_kernel_region_t region;
  create_kernel_region(&region);
  vmm_kernel_region_t **region_ptr = KERNEL_REGION_PTR_LOCATION;
  *region_ptr = &region;

  kio_printf("Hi 2\n");
  init_heap();
  kio_printf("Hi 3\n");

  init_cls();
  kio_printf("Hi 4\n");

  create_gdt();
  kio_printf("Hi 5\n");

  init_interrupts();

  kio_printf("Hi 6\n");
  init_x86_64_hal();

  kio_printf("Hi\n");
  while (1) {
  }
}
