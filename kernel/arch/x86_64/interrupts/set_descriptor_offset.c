#include <interrupts.h>

void set_descriptor_offset(idt_gate_descriptor_t *descriptor, uint64_t offset) {
  descriptor->offset_1 = offset & 0xFFFF;
  descriptor->offset_2 = (offset >> 16) & 0xFFFF;
  descriptor->offset_3 = (offset >> 32) & 0xFFFFFFFF;
}
