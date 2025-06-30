#ifndef X86_64_APIC_H
#define X86_64_APIC_H

#include <stddef.h>
#include <stdint.h>
#include <mem/pimemory.h>

#define APIC_REGISTERS_ADDR (0xFEE0000 + IDENTITY_MAPPED_ADDR)

#define SPURIOUS_INTERRUPT_VECTOR_REG 0xF0

uint32_t read_apic_register(size_t reg);
void write_apic_register(size_t reg, uint32_t value);

#endif
