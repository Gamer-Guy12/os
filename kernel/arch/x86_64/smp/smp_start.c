#include <asm.h>
#include <cls.h>
#include <gdt.h>
#include <hal/hal.h>
#include <interrupts.h>
#include <libk/kio.h>
#include <libk/math.h>
#include <libk/mem.h>
#include <libk/vga_kgfx.h>
#include <mem/kheap.h>
#include <mem/memory.h>
#include <mem/pimemory.h>
#include <mem/vimemory.h>
#include <multiboot.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <x86_64.h>

extern void create_local_proccess(void);

size_t setup_memory(void) {
  PML4_entry_t *pml4 =
      (PML4_entry_t *)((size_t)phys_alloc() + IDENTITY_MAPPED_ADDR);
  PML4_entry_t *cur_pml4 = (PML4_entry_t *)(PML4_ADDR);

  // Copy 256 (Physical structures)
  pml4[256] = cur_pml4[256];

  // Copy over 257 (Identity mapping)
  pml4[257] = cur_pml4[257];

  // Copy over 258 (Misc information)
  pml4[258] = cur_pml4[258];

  // Copy over 259 (Global Heap)
  pml4[259] = cur_pml4[259];

  // Recursive mapping
  size_t phys_pml4 = (size_t)pml4 - IDENTITY_MAPPED_ADDR;
  pml4[510].full_entry = phys_pml4;
  pml4[510].flags |= PT_PRESENT | PT_READ_WRITE;
  pml4[510].not_executable = 1;

  // Copy over 511 (Kernel)
  pml4[511] = cur_pml4[511];

  // Move it into cr3
  __asm__ volatile("mov %%rax, %%cr3" ::"a"(phys_pml4) : "memory");

  create_local_proccess();

  size_t old_page;
  __asm__ volatile("mov %%rsp, %%rax" : "=a"(old_page));
  old_page = ROUND_DOWN(old_page, PAGE_SIZE);

  return virt_to_phys(old_page);
}

void smp_start(size_t processor_id, size_t old_page) {

  __asm__ volatile("cli");

  phys_free((void *)old_page);

  init_heap();

  init_cls();

  create_gdt();

  init_interrupts();

  while (1) {
  }
}
