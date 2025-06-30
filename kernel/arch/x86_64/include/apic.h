#ifndef X86_64_APIC_H
#define X86_64_APIC_H

#include <stddef.h>
#include <stdint.h>

#define SPURIOUS_INTERRUPT_VECTOR_REG 0xF0

uint32_t read_register(size_t register);
void write_register(size_t register, uint32_t value);

#endif

