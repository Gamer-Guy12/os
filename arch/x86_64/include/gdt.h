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

enum gdt_ss_flags {
  SS_LDT = 0x2,
  SS_TSS_AVAIL = 0x9,
  SS_TSS_BUDY = 0xB,
};

struct system_segment {
  uint16_t limit_1;
  uint16_t base_1;
  uint8_t base_2;
  uint8_t access;
  uint8_t limit_2 : 4;
  uint8_t flags : 4;
  uint8_t base_3;
  uint32_t base_4;
  uint32_t reserved;
} __attribute__((packed));

struct tss {
  uint32_t reserved_1;
  uint64_t rsp0;
  uint64_t rsp1;
  uint64_t rsp2;
  uint64_t reserved_2;
  union {
    struct {
      uint64_t ist1;
      uint64_t ist2;
      uint64_t ist3;
      uint64_t ist4;
      uint64_t ist5;
      uint64_t ist6;
      uint64_t ist7;
    };
    uint64_t ist[7];
  };
  uint64_t reserved_3;
  uint16_t reserved_4;
  uint16_t iopb;
} __attribute__((packed));

// TSS, system segment, ring 0, present
#define TSS_ACCESS (0x9 | (0 << 4) | (0 << 5) | (1 << 7))

struct tss_segment {
  union {
    struct {
      uint16_t limit_1;
      uint16_t base_1;
      uint8_t base_2;
      uint8_t access;
      uint8_t limit_2 : 4;
      uint8_t flags : 4;
      uint8_t base_3;
      uint32_t base_4;
      uint32_t reserved;
    };
    struct {
      uint64_t low;
      uint64_t high;
    } __attribute__((packed));
  } __attribute__((packed));
} __attribute__((packed));

void init_gdt(void);
// A value of 0 will be put into the rsp field
void gdt_tss_stack(uint8_t ist, void *ptr);

#endif
