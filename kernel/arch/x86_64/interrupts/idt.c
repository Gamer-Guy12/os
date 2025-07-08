#include <cls.h>
#include <interrupts.h>
#include <libk/kio.h>
#include <stddef.h>
#include <stdint.h>

void set_idt_gate(uint8_t gate_number, void (*handler)(void),
                  uint16_t seg_descriptor, uint8_t gate_type, bool present,
                  uint8_t dpl, uint8_t ist) {
  idt_gate_descriptor_t *idt = get_cls()->idt;

  idt_gate_descriptor_t *descriptor = &idt[gate_number];

  descriptor->gate_type = gate_type;
  descriptor->selector = seg_descriptor;
  descriptor->dpl = dpl;
  descriptor->ist = ist;

  union {
    void (*handler)(void);
    void *offset;
  } converter;

  converter.handler = handler;

  set_descriptor_offset(descriptor, (size_t)converter.offset);
}

void load_idt(void) {
  idt_gate_descriptor_t *idt = get_cls()->idt;

  idt_descriptor_t descriptor;
  descriptor.size = sizeof(idt_gate_descriptor_t) * 256 - 1;
  descriptor.offset = (size_t)idt;

  // Interrupts no work without this
  kio_printf("");

  __asm__ volatile("lidt (%0)" : : "r"(&descriptor));
}

void enable_idt_gate(uint8_t gate_number) {
  idt_gate_descriptor_t *idt = get_cls()->idt;
  idt[gate_number].present = true;
}
