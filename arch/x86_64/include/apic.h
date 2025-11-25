#ifndef _x86_64_APIC_H_
#define _x86_64_APIC_H_

#include <stdbool.h>
#include <stdint.h>

#define LAPIC_BASE 0xFEE00000

#define LAPIC_ID_REG 0x20
#define LAPIC_VERSION_REG 0x30
#define LAPIC_TPR_REG 0x80
#define LAPIC_APR_REG 0x90
#define LAPIC_PPR_REG 0xA0
#define LAPIC_EOI_REG 0xB0
#define LAPIC_RRD_REG 0xC0
#define LAPIC_LOGICAL_DEST_REG 0xD0
#define LAPIC_DEST_FORMAT_REG 0xE0
#define LAPIC_SVR_REG 0xF0
#define LAPIC_ESR_REG 0x280
#define LAPIC_LVT_CMCI_REG 0x2F0
#define LAPIC_ICR_LOW_REG 0x300
#define LAPIC_ICR_HIGH_REG 0x310
#define LAPIC_LVT_TIMER_REG 0x320
#define LAPIC_LVT_THERMAL_REG 0x330
#define LAPIC_LVT_PERF_MON_REG 0x340
#define LAPIC_LVT_LINT0_REG 0x350
#define LAPIC_LVT_LINT1_REG 0x360
#define LAPIC_LVT_ERR_REG 0x370
#define LAPIC_TIMER_INIT_COUNT_REG 0x380
#define LAPIC_TIMER_CUR_COUNT_REG 0x390
#define LAPIC_DIV_CONFIG_REG 0x3E0

bool check_apic(void);
void enable_apic(void);
void write_apic_reg(uint16_t reg, uint32_t val);
uint32_t read_apic_reg(uint16_t reg);
void apic_eoi(void);
void init_apic_timer(void);
// Waits for ipi to finish
void send_ipi(uint32_t cpuid, uint8_t interrupt);
void write_ioapic(uint32_t reg, uint32_t value);
uint32_t read_ioapic(uint32_t reg);
void configure_ioapic_entry(uint8_t interrupt, uint8_t delivery_mode,
                            bool logical_dest, bool active_low,
                            bool level_triggered, bool masked, uint8_t irq);
void mask_ioapic_irq(uint8_t irq);
void unmask_ioapic_irq(uint8_t irq);
// Take ISA irq and get its real value you should map
uint8_t get_real_irq(uint8_t irq);

#define IOAPIC_IRQ(interrupt, irq)                                             \
  configure_ioapic_entry(interrupt, 0, 0, 0, 0, 1, irq)

#endif
