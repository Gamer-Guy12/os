#include "apic.h"
#include "acpi.h"
#include "asm.h"
#include "interrupts.h"
#include "kernel/cores.h"
#include "kernel/kprintf.h"
#include "kernel/mem.h"
#include "kernel/timers.h"
#include "lib/atomic.h"
#include "util.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define IA32_APIC_BASE_MSR 0x1B

static uintptr_t lapic_addr = 0;
static volatile uint32_t *ioapic_addr = NULL;

CLS(uint64_t, timer_frequency);

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

  ioapic_addr = map_phys((void *)(uint64_t)ioapic_entry->ioapic_addr,
                         PM_RW | PM_UNCACHEABLE | PM_PINNED);

  if (is_bsp()) {
    // Mask all interrupts 24
    for (int i = 0; i < 24; i++) {
      mask_ioapic_irq(i);
    }
  }
  apic_eoi();

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
  uint32_t reg = read_ioapic(0x10 + irq * 2);
  reg |= (1 << 16);
  write_ioapic(0x10 + irq * 2, reg);
}

void unmask_ioapic_irq(uint8_t irq) {
  uint32_t reg = read_ioapic(0x10 + irq * 2);
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

CLS(atomic_t, apic_calculated);
CLS(uint64_t, end_ticks);

typedef void (*handler_t)(void);

CLS(handler_t, handlers);

static void apic_handler(void *data) {
  apic_eoi();

  atomic_t *this_apic = GET_CLS(apic_calculated);
  if (atomic_load(this_apic) == 0) {
    uint64_t *this_end = GET_CLS(end_ticks);
    *this_end = abs_time();

    atomic_store(this_apic, 1);
  } else {
    handler_t *local_handler = GET_CLS(handlers);
    (*local_handler)();
  }
}

uint32_t apic_ticks(uint32_t ms) {
  uint64_t *this_freq = GET_CLS(timer_frequency);

  return ms * *this_freq / 1000;
}

void apic_wait_ms(void (*handler)(void), uint32_t ms) {
  uint32_t ticks = apic_ticks(ms);
  handler_t *this_handler = GET_CLS(handlers);
  *this_handler = handler;

  write_apic_reg(LAPIC_DIV_CONFIG_REG, DIV_4);
  // Interrupt 0x60, not masked, one shot
  write_apic_reg(LAPIC_LVT_TIMER_REG, 0x60);
  write_apic_reg(LAPIC_TIMER_INIT_COUNT_REG, ticks);
}

void apic_tick_handler(void);

INIT void init_apic_timer(void) {
  enable_interrupts();
  uint64_t *freq = GET_CLS(timer_frequency);
  write_apic_reg(LAPIC_DIV_CONFIG_REG, DIV_4);
  // Interrupt 0x60, not masked, one shot
  register_int_handler(0x60, apic_handler);
  write_apic_reg(LAPIC_LVT_TIMER_REG, 0x60);
  // Wait this many ticks and then see how many ms that is

  uint64_t start_ticks = abs_time();
  write_apic_reg(LAPIC_TIMER_INIT_COUNT_REG, 16384);

  atomic_t *this_apic = GET_CLS(apic_calculated);
  while (!atomic_load(this_apic)) {
  }

  uint64_t *this_end = GET_CLS(end_ticks);
  uint64_t tick_diff = *this_end - start_ticks;
  // Ticks per second
  const uint64_t abs_frequency = abs_freq();
  // Always use the div 4 divider
  const uint64_t lapic_freq = abs_frequency * 16384 / tick_diff;
  *freq = lapic_freq;

  // Start ticking
  apic_wait_ms(apic_tick_handler, TICK_LEN_MS);
}

// The apic timer will be the only one handling ticks, if that doesn't work then
// womp womp
void apic_tick_handler(void) {
  apic_wait_ms(apic_tick_handler, TICK_LEN_MS);
  increment_tick();
}
