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

  struct tss_segment tss_seg;
  tss_seg.access = TSS_ACCESS;
  tss_seg.flags = 0;

  size_t tss_size = sizeof(struct tss);
  tss_seg.limit_1 = (tss_size - 1) & 0xFFFF;
  tss_seg.limit_2 = ((tss_size - 1) >> 16) & 0xF;
  
  uintptr_t tss_addr = (uintptr_t)&tss;
  tss_seg.base_1 = tss_addr & 0xFFFF;
  tss_seg.base_2 = (tss_addr >> 16) & 0xFF;
  tss_seg.base_3 = (tss_addr >> 24) & 0xFF;
  tss_seg.base_4 = (tss_addr >> 32) & 0xFFFFFFFF;

  core_gdt[5] = tss_seg.low;
  core_gdt[6] = tss_seg.high;

  gdtptr_t ptr = {.addr = (uint64_t)core_gdt, .size = sizeof(uint64_t) * 7 - 1};

  __asm__ volatile("lgdt (%0)" ::"r"(&ptr));
  __asm__ volatile("ltr %%ax" :: "a"(0x28));

  __asm__ volatile("mov %%ax, %%ds" :: "a"(0x10));
  __asm__ volatile("mov %%ax, %%es" :: "a"(0x10));
  __asm__ volatile("mov %%ax, %%ss" :: "a"(0x10));
}
