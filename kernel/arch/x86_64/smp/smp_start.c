#include <apic_timer.h>
#include <asm.h>
#include <cls.h>
#include <gdt.h>
#include <hal/hal.h>
#include <interrupts.h>
#include <libk/kio.h>
#include <libk/math.h>
#include <libk/mem.h>
#include <libk/vga_kgfx.h>
#include <mem/memory.h>
#include <mem/pimemory.h>
#include <mem/vimemory.h>
#include <multiboot.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <threading.h>
#include <threading/threading.h>
#include <x86_64.h>

extern void create_local_proccess(void);

typedef struct {
  size_t phys_addr;
  size_t stack_addr;
} setup_ret_t;

setup_ret_t setup_memory(void) {
  PML4_entry_t *pml4 =
      (PML4_entry_t *)((size_t)phys_alloc() + IDENTITY_MAPPED_ADDR);
  PML4_entry_t *cur_pml4 = (PML4_entry_t *)(PML4_ADDR);

  for (size_t i = 256; i < 512; i++) {
    if (i != 510)
      pml4[i] = cur_pml4[i];
  }

  // Recursive mapping
  size_t phys_pml4 = (size_t)pml4 - IDENTITY_MAPPED_ADDR;
  pml4[510].full_entry = phys_pml4;
  pml4[510].flags |= PT_PRESENT | PT_READ_WRITE;
  pml4[510].not_executable = 1;

  // Move it into cr3
  __asm__ volatile("mov %%rax, %%cr3" ::"a"(phys_pml4) : "memory");

  create_local_proccess();

  size_t old_page;
  __asm__ volatile("mov %%rsp, %%rax" : "=a"(old_page));
  old_page = ROUND_DOWN(old_page, PAGE_SIZE);

  MFENCE;
  setup_ret_t ret = {.phys_addr = virt_to_phys(old_page),
                     .stack_addr = ((TCB_t *)(TCB))->rsp0};

  return ret;
}

void smp_start(size_t processor_id, size_t old_page, size_t feature_flags) {

  CLI;

  phys_free((void *)old_page);

  init_cls(feature_flags);

  create_gdt();

  init_interrupts();

  init_threading();

  init_apic_timer();
  start_preemption();
  enable_preemption();

  kill_cur_thread();
}
