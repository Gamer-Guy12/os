#ifndef _x86_64_GDT_H_
#define _x86_64_GDT_H_

#include <stdint.h>

typedef struct {
  // Size - 1
  uint16_t size;
  uint64_t addr;
} __attribute__((packed)) gdtptr_t;

enum gdt_access {
  GDT_ACCESSED = (1 << 0),
  GDT_RW = (1 << 1),
  // Direction/Conforming
  GDT_DC = (1 << 2),
  GDT_EXEC = (1 << 3),
  // Set for Data or Code segment
  GDT_TYPE = (1 << 4),
  GDT_DPL_0 = (0 << 5),
  GDT_DPL_1 = (1 << 5),
  GDT_DPL_2 = (2 << 5),
  GDT_DPL_3 = (3 << 5),
  GDT_PRESENT = (1 << 7)
};

enum gdt_flags {
  GDT_LONG = (1 << 1),
  // Only set for data segment
  GDT_DB = (1 << 2),
  // Always set
  GDT_GRANULARITY = (1 << 3)
};

struct gdt_segment {
  union {
    struct {
      uint16_t limit_1;
      uint16_t base_1;
      uint8_t base_2;
      uint8_t access_byte;
      uint8_t limit_2 : 4;
      uint8_t flags : 4;
      uint8_t base_3;
    };
    uint64_t segment;
  };
} __attribute__((packed));

void init_gdt(void);

#endif
