#ifndef X86_64_INTERRUPTS
#define X86_64_INTERRUPTS

#include <decls.h>
#include <stdint.h>

#define IDT_INTERRUPT_GATE 0b1110
#define IDT_TRAP_GATE 0b1111

typedef struct {
  uint16_t offset_1;
  uint16_t selector;
  uint8_t ist : 3;
  uint8_t reserved_1 : 5;
  uint8_t gate_type : 4;
  uint8_t reserved_2 : 1;
  uint8_t dpl : 2;
  uint8_t present : 1;
  uint16_t offset_2;
  uint32_t offset_3;
  uint32_t reserved_3;
} PACKED idt_gate_descriptor_t;

typedef struct {
  /// Size - 1
  uint16_t size;
  uint64_t offset;
} PACKED idt_descriptor;

void set_descriptor_offset(idt_gate_descriptor_t *descriptor, uint64_t offset);

#endif
