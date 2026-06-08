#ifndef _x86_64_GDT_H_
#define _x86_64_GDT_H_

#include <stdint.h>

typedef struct {
  // Size - 1
  uint16_t size;
  uint64_t addr;
} __attribute__((packed)) gdt_descriptor_t;

enum gdt_access {
  // Was this accessed
  GDT_ACCESSED = (1 << 0),
  // Is this read write
  GDT_RW = (1 << 1),
  // For data segments leave this to 0
  // For code segments leave this to 0
  GDT_CONFORM = (1 << 2),
  // If set to 1 this is a code segment
  GDT_EXEC = (1 << 3),
  // 0 means a system descriptor (tss), 1 means it isn't
  GDT_TYPE = (1 << 4),

  GDT_DPL_0 = (0 << 5),
  GDT_DPL_1 = (1 << 5),
  GDT_DPL_2 = (2 << 5),
  GDT_DPL_3 = (3 << 5),

  GDT_PRESENT = (1 << 7),

  // For System Segments
  GDT_TYPE_LDT = 0x2,
  GDT_TYPE_TSS_AVAIL = 0x9,
  GDT_TYPE_TSS_BUSY = 0xB
};

enum gdt_flags {
  // Defines a long mode CODE segment
  GDT_LONG = (1 << 1),
  // Set this when defining a DATA segment otherwise don't
  GDT_SIZE = (1 << 2),
  // Just set this to 1 all the time
  GDT_GRANULARITY = (1 << 3)
};

struct gdt_segment {
  union {
    struct {
      uint64_t limit_0 : 16;
      uint64_t base_0 : 16;
      uint64_t base_1 : 8;
      uint64_t access : 8;
      uint64_t limit_1 : 4;
      uint64_t flags : 4;
      uint8_t base : 8;
    } __attribute__((packed));
    uint64_t value;
  } __attribute__((packed));
} __attribute__((packed));

struct gdt_system {
  union {
    struct {
      uint64_t limit_0 : 16;
      uint64_t base_0 : 16;
      uint64_t base_1 : 8;
      uint64_t access : 8;
      uint64_t limit_1 : 4;
      uint64_t flags : 4;
      uint64_t base_2 : 8;
      uint64_t base_3 : 32;
      uint64_t padding : 32;
    } __attribute__((packed));
    uint64_t value[2];
  } __attribute__((packed));
} __attribute__((packed));

struct tss {
  uint32_t reserved_0;
  uint64_t rsp0;
  uint64_t rsp1;
  uint64_t rsp2;
  uint64_t reserved_1;
  uint64_t ist1;
  uint64_t ist2;
  uint64_t ist3;
  uint64_t ist4;
  uint64_t ist5;
  uint64_t ist6;
  uint64_t ist7;
  uint64_t reserved_2;
  uint16_t reserved_3;
  uint16_t iobp;
} __attribute__((packed));

#endif
