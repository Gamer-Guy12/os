#include "apic.h"
#include "interrupts.h"
#include "acpi.h"
#include "asm.h"
#include "kernel/kprintf.h"
#include "kernel/mem.h"
#include "util.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define IA32_APIC_BASE_MSR 0x1B

static uintptr_t lapic_addr = 0;
static volatile uint32_t *ioapic_addr = NULL;

INIT bool check_apic(void) {
  uint32_t a, d;
  cpuid(1, &a, &d);
  return d & (1 << 9);
}

INIT void disable_pic(void) {
  OUTB(0x20, 0x11);
  IO_WAIT();
  OUTB(0xA0, 0x11);
  IO_WAIT();
  OUTB(0x21, 0x20);
  IO_WAIT();
  OUTB(0xA1, 0x28);
  IO_WAIT();
  OUTB(0x21, 4);
  IO_WAIT();
  OUTB(0xA1, 2);
  IO_WAIT();

  OUTB(0x21, 0x01);
  IO_WAIT();
  OUTB(0xA1, 0x01);
  IO_WAIT();

  OUTB(0x21, 0xFF);
  OUTB(0xA1, 0xFF);
}

INIT void enable_apic(void) {
  disable_interrupts();
  if (!check_apic()) {
    kprintf("No Apic On Chip\n");
    panic();
  }

  disable_pic();

  // Enable with apic base address
  WRMSR(IA32_APIC_BASE_MSR, LAPIC_BASE | (1 << 11));

  lapic_addr =
      (uintptr_t)map_phys((void *)0xFEE00000, PM_WRITE_THROUGH | PM_RW);

  // Enable the apic in the SVR
  // The vector will be 0xFF
  write_apic_reg(LAPIC_SVR_REG, 0xFF | (1 << 8));
  write_apic_reg(LAPIC_TPR_REG, 0);

  struct madt *madt = get_acpi_table(MADT);
  size_t size_left = madt->header.length - sizeof(struct madt);
  uintptr_t cur_ptr = (uintptr_t)madt + sizeof(struct madt);
  struct madt_ioapic *ioapic_entry = NULL;

  while (size_left > 0) {
    struct madt_entry *header = (void *)cur_ptr;

    if (header->type == 1) {
      ioapic_entry = (void *)header;
      break;
    }

    size_left -= header->length;
    cur_ptr += header->length;
  }

  if (ioapic_entry == NULL) {
    kprintf("No IOAPIC\n");
    panic();
  }

  ioapic_addr = (void *)(uint64_t)ioapic_entry->ioapic_addr;
  enable_interrupts();
}

void write_apic_reg(uint16_t reg, uint32_t val) {
  volatile uint32_t *write_addr = (void *)(lapic_addr + reg);

  *write_addr = val;
}

uint32_t read_apic_reg(uint16_t reg) {
  volatile uint32_t *read_addr = (void *)(lapic_addr + reg);
  return *read_addr;
}

void apic_eoi(void) { write_apic_reg(LAPIC_EOI_REG, 0); }

void send_ipi(uint32_t cpuid, uint8_t interrupt) {
  write_apic_reg(LAPIC_ICR_HIGH_REG, cpuid);
  write_apic_reg(LAPIC_ICR_LOW_REG, interrupt);
}

void write_ioapic(uint32_t reg, uint32_t value) {
  ioapic_addr[0] = (reg & 0xFF);
  ioapic_addr[4] = value;
}

uint32_t read_ioapic(uint32_t reg) {
  ioapic_addr[0] = (reg & 0xFF);
  return ioapic_addr[4];
}

void configure_ioapic_entry(uint8_t interrupt, uint8_t delivery_mode,
                            bool logical_dest, bool active_low,
                            bool level_triggered, bool masked, uint8_t irq) {
  uint64_t entry = 0;
  entry |= interrupt;
  entry |= (delivery_mode & 0x7) << 8;
  entry |= (logical_dest & 0x1) << 11;
  entry |= (active_low & 0x1) << 13;
  entry |= (level_triggered & 0x1) << 15;
  entry |= (masked & 0x1) << 16;

  write_ioapic(0x10 + irq * 2, entry);
  write_ioapic(0x11 + irq * 2, entry >> 32);
}

void mask_ioapic_irq(uint8_t irq) {
  configure_ioapic_entry(0, 0, 0, 0, 0, 1, irq);
}

void unmask_ioapic_irq(uint8_t irq) {
  uint32_t reg = read_apic_reg(0x10 + irq * 2);
  reg &= ~(1 << 16);
  write_ioapic(0x10 + irq * 2, reg);
}

uint8_t get_real_irq(uint8_t irq) {
  struct madt *madt = get_acpi_table(MADT);
  size_t size_left = madt->header.length - sizeof(struct madt);
  uintptr_t cur_ptr = (uintptr_t)madt + sizeof(struct madt);

  while (size_left > 0) {
    struct madt_entry *entry = (void *)cur_ptr;

    if (entry->type == 2) {
      struct madt_ioapic_override *override = (void *)entry;
      if (override->irq_source == irq) {
        return override->gsi_interrupt;
      }
    }

    cur_ptr += entry->length;
    size_left -= entry->length;
  }

  return irq;
}

void init_apic_timer(void) {}
