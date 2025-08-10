#ifndef X86_64_APIC_H
#define X86_64_APIC_H

#include <stddef.h>
#include <stdint.h>
#include <mem/pimemory.h>

#define APIC_REGISTERS_ADDR (0xFEE00000 + IDENTITY_MAPPED_ADDR)

#define SPURIOUS_INTERRUPT_VECTOR_REG 0xF0
#define EOI_REG 0xB0
#define TASK_PRIORITY_REG 0x80
#define APIC_ID_REG 0x20
#define ERROR_STATUS_REG 0x280
#define INTERRUPT_COMMAND_REG_1 0x300
#define INTERRUPT_COMMAND_REG_2 0x310
#define LVT_TIMER_REG 0x320
#define TIMER_INITIAL_COUNT_REG 0x380
#define TIMER_CUR_COUNT_REG 0x390
#define TIMER_DIV_CONFIG_REG 0x3E0

#define LVT_VECTOR(num) (num & 0xff)
#define LVT_NMI (0x400)
#define LVT_INT_PENDING (1 << 12)
#define LVT_LOW_TRIGGERED (1 << 13)
#define LVT_REMOTE_IRR (1 << 14)
#define LVT_LEVEL_TRIG (1 << 15)
#define LVT_MASK (1 << 16)

#define TIMER_DIV_2 (0x0)
#define TIMER_DIV_4 (0x1)
#define TIMER_DIV_8 (0x2)
#define TIMER_DIV_16 (0x3)
#define TIMER_DIV_32 (0x8)
#define TIMER_DIV_64 (0x9)
#define TIMER_DIV_128 (0xA)
#define TIMER_DIV_1 (0xB)

#define TIMER_ONE_SHOT (0x0)
#define TIMER_PERIODIC ((0x1) << 17)
#define TIMER_TSC ((0x2) << 17)

#define IA32_APIC_BASE 0x1B

uint32_t read_apic_register(size_t reg);
void write_apic_register(size_t reg, uint32_t value);

#endif
