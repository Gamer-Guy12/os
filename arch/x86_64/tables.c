#include "gdt.h"
#include "idt.h"
#include "kernel/cores.h"
#include "kernel/mem.h"
#include "util.h"
#include <stdint.h>

#define ENTRY_COUNT 7

// 1 Null entry
// Ring 0 Code
// Ring 0 Data
// Ring 3 Data
// Ring 3 Code
// TSS Part 1
// TSS Part 2
CLS(uint64_t[ENTRY_COUNT], gdt);
CLS(struct tss, tss);

static void init_gdt(void) {
  uint64_t *core_gdt = GET_CLS(gdt);

  // Null segment
  core_gdt[0] = 0;

  struct gdt_segment segment;

  // Ring 0 Code
  segment.value = 0;
  segment.limit_0 = 0xFFFF;
  segment.limit_1 = 0xF;
  segment.access = GDT_ACCESSED | GDT_EXEC | GDT_TYPE | GDT_DPL_0 | GDT_PRESENT;
  segment.flags = GDT_LONG | GDT_GRANULARITY;
  core_gdt[1] = segment.value;

  // Ring 0 Data
  segment.value = 0;
  segment.limit_0 = 0xFFFF;
  segment.limit_1 = 0xF;
  segment.access = GDT_ACCESSED | GDT_RW | GDT_TYPE | GDT_DPL_0 | GDT_PRESENT;
  segment.flags = GDT_SIZE | GDT_GRANULARITY;
  core_gdt[2] = segment.value;

  // Ring 3 Data
  segment.value = 0;
  segment.limit_0 = 0xFFFF;
  segment.limit_1 = 0xF;
  segment.access = GDT_ACCESSED | GDT_RW | GDT_TYPE | GDT_DPL_3 | GDT_PRESENT;
  segment.flags = GDT_SIZE | GDT_GRANULARITY;
  core_gdt[3] = segment.value;

  // Ring 3 Code
  segment.value = 0;
  segment.limit_0 = 0xFFFF;
  segment.limit_1 = 0xF;
  segment.access = GDT_ACCESSED | GDT_RW | GDT_TYPE | GDT_DPL_3 | GDT_PRESENT;
  segment.flags = GDT_SIZE | GDT_GRANULARITY;
  core_gdt[4] = segment.value;

  // TSS
  struct gdt_system system;
  struct tss *core_tss = GET_CLS(tss);
  // Allocate a stack that can be used in the case of errors
  uintptr_t stack_addr = (uint64_t)_alloc_pages(2, ZONE_ANY);
  stack_addr += 2 * PAGE_SIZE;
  core_tss->ist1 = stack_addr;
  uintptr_t tss_addr = (uintptr_t)core_tss;

  system.access = GDT_TYPE_TSS_AVAIL | GDT_PRESENT;
  system.base_0 = tss_addr & 0xFFFF;
  system.base_1 = (tss_addr >> 16) & 0xFF;
  system.base_2 = (tss_addr >> 24) & 0xFF;
  system.base_3 = (tss_addr >> 32) & 0xFFFFFFFF;

  system.limit_0 = (sizeof(struct tss) - 1) & 0xFFFF;
  system.limit_1 = ((sizeof(struct tss) - 1) >> 16) & 0xF;

  core_gdt[5] = system.value[0];
  core_gdt[6] = system.value[1];

  gdt_descriptor_t desc = {.size = sizeof(uint64_t) * ENTRY_COUNT - 1,
                           .addr = (uintptr_t)core_gdt};
  __asm__ volatile("lgdt (%0)" ::"r"(&desc) : "memory");
  __asm__ volatile("ltr %%ax" ::"a"(0x28));

  __asm__ volatile("mov %%ax, %%ds" ::"a"(0x10));
  __asm__ volatile("mov %%ax, %%es" ::"a"(0x10));
  __asm__ volatile("mov %%ax, %%ss" ::"a"(0x10));
  // Switch code segments
  __asm__ volatile("pushq $0x08\n"
                   "leaq 1f(%%rip), %%rax\n"
                   "pushq %%rax\n"
                   "lretq\n"
                   "1:\n" ::
                       : "rax", "memory");
}

static void init_tables(void) {
  init_gdt();
  init_idt();
}
INITFUNC(init_tables, CALL_CLS);
