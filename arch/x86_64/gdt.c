#include "gdt.h"
#include "init.h"
#include "./init.h"
#include "kernel/cores.h"

CLS(uint64_t[7], gdt);

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

  gdtptr_t ptr = {
    .addr = (uint64_t)core_gdt,
    .size = sizeof(uint64_t) * 7 - 1
  };

  __asm__ volatile("lgdt (%0)" :: "r"(&ptr));
}

