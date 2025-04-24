#ifndef X86_64_INTERUPT
#define X86_64_INTERUPT

#include <stdint.h>

#define INTERRUPT_GATE 0xe
#define TRAP_GATE 0xf

#define DPL_RING_0 0
#define DPL_RING_1 1 << 5
#define DPL_RING_2 2 << 5
#define DPL_RING_3 3 << 5

#define ENTRY_PRESENT 1 << 7
#define ENTRY_NOT_PRESENT 0

typedef struct {
  uint16_t offset_1;
  uint16_t selector;
  uint8_t ist;
  uint8_t types;
  uint16_t offset_2;
  uint32_t offset_3;
  uint32_t reserved;
} __attribute__((packed)) interupt_descriptor_t;

#endif
