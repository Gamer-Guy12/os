#ifndef _x86_64_APIC_H_
#define _x86_64_APIC_H_

#include <stdbool.h>
#include <stdint.h>

#define APIC_MSR_BASE 0x800

// LAPIC ID
#define LAPIC_ID_REG 0x2
// LAPIC Version
#define LAPIC_VER_REG 0x3
// Task Priority
#define LAPIC_TPR_REG 0x8
// Processor Priority
#define LAPIC_PPR_REG 0xA
// End of Interrupt
#define LAPIC_EOI_REG 0xB
// Logical Destination
#define LAPIC_LDR_REG 0xD
// Spurious Interrupt Vector
#define LAPIC_SVR_REG 0xF
// In-Service Register
#define LAPIC_ISR_REG 0x10
// Trigger Mode Register
#define LAPIC_TMR_REG 0x18
// Interrupt Request Register
#define LAPIC_IRR_REG 0x20
// Error Status Register
#define LAPIC_ESR_REG 0x28
// Interrupt Commnad Register
#define LAPIC_ICR_REG 0x30
// LVT Timer Register
#define LAPIC_TIMER_REG 0x32
// LVT Thermal Sensor Register
#define LAPIC_THERMAL_SENSOR_REG 0x33
// LVT Performance Monitoring Counters Register
#define LAPIC_PERF_MONITOR_REG 0x34
// LVT LINT0 Register
#define LAPIC_LINT0_REG 0x35
// LVT LINT1 Register
#define LAPIC_LINT1_REG 0x36
// LVT Error Register
#define LAPIC_LVT_ERR_REG 0x37
// Timer Initial Count
#define LAPIC_TIC_REG 0x38
// Timer Current Count
#define LAPIC_TCC_REG 0x39
// Timer Divide Configuration
#define LAPIC_TDC_REG 0x3E
// Self IPI
#define LAPIC_SELF_IPI_REG 0x3F

enum ipi_type {
  // Sends it to the target
  IPI_NORMAL,
  // Sends it to everyone except the sender
  IPI_BROADCAST,
  // Sends it to everyone including the sender
  IPI_ALL
};

// Interrupts don't have to set up yet because register_interrupts does not
// require the IDT to be created

// Implemented in cores.c (because this only supports the x2apic and no earlier)
// All intel core cpus support x2apic and amd zen 2 and later support it (a lot of servers also do)
void check_apic(void);
void enable_apic(void);
void init_apic(void);
// Checks if the apic has been initialized
bool apic_up(void);

// Usage
void apic_write(uint16_t reg, uint64_t value);
uint64_t apic_read(uint16_t reg);
void apic_eoi(void);
// dest is ignored when type isn't IPI_NORMAL
void apic_ipi(int type, uint32_t dest, uint8_t interrupt);

#endif
