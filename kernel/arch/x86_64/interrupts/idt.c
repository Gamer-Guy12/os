#include <interrupts.h>

ALIGN(0x10) idt_gate_descriptor_t idt[256];

void common_interrupt_handler(idt_registers_t *registers) {}
