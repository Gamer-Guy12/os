#ifndef _x86_64_INTERRUPTS_H_
#define _x86_64_INTERRUPTS_H_

#include <stdint.h>

typedef struct {
  // Size - 1
  uint16_t size;
  uint64_t addr;
} __attribute__((packed)) idtptr_t;

struct idt_descriptor {
  uint16_t offset_1;
  uint16_t segment;
  uint16_t ist : 3;
  uint16_t reserved_1 : 5;
  uint8_t gate_type : 4;
  uint8_t reserved_2 : 1;
  uint8_t dpl : 2;
  uint8_t present : 1;
  uint16_t offset_2;
  uint32_t offset_3;
  uint32_t reserved_3;
} __attribute__((packed));

#endif

