#include "gdt.h"
#include "kernel/mem.h"
#include "kernel/cores.h"
#include <stdint.h>

CLS(uint64_t[7], gdt);

struct tss tss;

void gdt_tss_stack(uint8_t ist, void* ptr) {
  if (ist == 0) {
    tss.rsp0 = (uint64_t)ptr;
  } else {
    tss.ist[ist] = (uint64_t)ptr;
  }

  uint64_t *core_gdt = GET_CLS(gdt);
  core_gdt[5] = tss.segment_low;
  core_gdt[6] = tss.segment_high;
}

void init_gdt(void) {
  uint64_t *core_gdt = GET_CLS(gdt);

  // NULL Segment
  core_gdt[0] = 0;

  struct gdt_segment segment;

  // Kernel Code Segment
  segment.flags = GDT_LONG | GDT_GRANULARITY;
  segment.limit_1 = 0xFFFF;
  segment.limit_2 = 0xF;
  segment.access_byte = GDT_PRESENT | GDT_DPL_0 | GDT_TYPE | GDT_EXEC | GDT_RW;

  core_gdt[1] = segment.segment;

  // Kernel Data Segment
  segment.flags = GDT_DB | GDT_GRANULARITY;
  segment.limit_1 = 0xFFFF;
  segment.limit_2 = 0xF;
  segment.access_byte = GDT_PRESENT | GDT_DPL_0 | GDT_TYPE | GDT_RW;

  core_gdt[2] = segment.segment;

  // User Data Segment
  segment.flags = GDT_DB | GDT_GRANULARITY;
  segment.limit_1 = 0xFFFF;
  segment.limit_2 = 0xF;
  segment.access_byte = GDT_PRESENT | GDT_DPL_3 | GDT_TYPE | GDT_RW;

  core_gdt[3] = segment.segment;

  // User Code Segment
  segment.flags = GDT_LONG | GDT_GRANULARITY;
  segment.limit_1 = 0xFFFF;
  segment.limit_2 = 0xF;
  segment.access_byte = GDT_PRESENT | GDT_DPL_3 | GDT_TYPE | GDT_EXEC | GDT_RW;

  core_gdt[4] = segment.segment;

  // Put a stack into ist1
  // This stack will never be destoryed
  void *stack = alloc_pages(3, ZONE_ANY);
  uintptr_t stack_ptr = (uintptr_t)stack + PAGE_SIZE * (1 << 3);
  gdt_tss_stack(1, (void*)stack_ptr);

  core_gdt[5] = tss.segment_low;
  core_gdt[6] = tss.segment_high;

  gdtptr_t ptr = {.addr = (uint64_t)core_gdt, .size = sizeof(uint64_t) * 7 - 1};

  __asm__ volatile("lgdt (%0)" ::"r"(&ptr));

  __asm__ volatile("mov %%ax, %%es" :: "a"(0x10));
  __asm__ volatile("mov %%ax, %%ss" :: "a"(0x10));
}
