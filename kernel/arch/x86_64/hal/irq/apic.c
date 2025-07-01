#include <acpi/acpi.h>
#include <apic.h>
#include <asm.h>
#include <hal/irq.h>
#include <interrupts.h>
#include <mem/pimemory.h>
#include <stddef.h>
#include <stdint.h>

bool check_apic(void) {
  uint32_t a, d;
  cpuid(1, &a, &d);
  return d & (1 << 9);
}

uint32_t get_apic_id(void) { return read_apic_register(APIC_ID_REG); }

void *get_io_apic_addr(void) {
  MADT_t *madt = acpi_get_struct("APIC");
  size_t size_left = madt->header.length - sizeof(MADT_t);

  while (size_left > 0) {
    MADT_entry_header_t *header =
        (MADT_entry_header_t *)((uint8_t *)madt +
                                (madt->header.length - size_left));

    if (header->entry_type == 1) {
      MADT_entry_1_t *entry = (MADT_entry_1_t *)header;
      return (void *)(size_t)(entry->io_apic_addr + IDENTITY_MAPPED_ADDR);
    } else {
      if (header->record_length > size_left) {
        return NULL;
      }
      size_left -= header->record_length;
      continue;
    }
  }

  return NULL;
}

void apic_eoi(void) { write_apic_register(EOI_REG, 0); }

uint32_t get_apic_irq_from_isa(uint8_t isa_irq) {
  MADT_t *madt = acpi_get_struct("APIC");
  size_t size_left = madt->header.length - sizeof(MADT_t);

  while (size_left > 0) {
    MADT_entry_header_t *header =
        (MADT_entry_header_t *)((uint8_t *)madt +
                                (madt->header.length - size_left));

    if (header->entry_type == 2) {
      MADT_entry_2_t *entry = (MADT_entry_2_t *)header;

      if (entry->irq_source == isa_irq) {
        return entry->global_system_interrupt;
      }
    }

    if (size_left < header->record_length) {
      return isa_irq;
    }

    size_left -= header->record_length;
  }

  return isa_irq;
}

void write_io_apic_reg(uint32_t reg, uint32_t value) {
  uint32_t *ioapic = get_io_apic_addr();

  ioapic[0] = reg;
  ioapic[4] = value;
}

uint32_t read_io_apic_reg(uint32_t reg) {
  uint32_t *ioapic = get_io_apic_addr();

  ioapic[0] = reg;
  return ioapic[4];
}

void apic_map_irq(uint8_t interrupt_number, uint8_t irq_number) {
  uint32_t actual_irq = get_apic_irq_from_isa(irq_number);

  uint32_t redirect_lobyte = 0;
  uint32_t redirect_hibyte = 0;

  // Mask irq
  redirect_lobyte |= (1 << 16);
  // Set interrupt_number
  redirect_lobyte |= interrupt_number;

  // Set lapic id
  uint8_t apic_id = get_apic_id();
  redirect_hibyte |= (apic_id << 24);

  size_t offset = 0x10 + actual_irq * 2;

  write_io_apic_reg(offset, redirect_lobyte);
  write_io_apic_reg(offset + 1, redirect_hibyte);

  enable_idt_gate(interrupt_number);
}

void apic_mask_irq(uint32_t irq) {
  uint32_t actual_irq = get_apic_irq_from_isa(irq);
  size_t offset = 0x10 + actual_irq * 2;

  uint32_t redirect_lobyte = read_apic_register(offset);

  redirect_lobyte |= (1 << 16);

  write_io_apic_reg(offset, redirect_lobyte);
}

void apic_unmask_irq(uint32_t irq) {
  uint8_t actual_irq = get_apic_irq_from_isa(irq);
  uint32_t offset = 0x10 + actual_irq * 2;

  uint32_t redirect_lobyte = read_io_apic_reg(offset);

  redirect_lobyte &= ~(1 << 16);

  write_io_apic_reg(offset, redirect_lobyte);
}

void apic_mask_all_irqs(void) {
  for (size_t i = 0; i < 24; i++) {
    uint32_t redirect_lobyte = read_apic_register(0x10 + i * 2);

    redirect_lobyte |= 1 << 16;

    write_io_apic_reg(0x10 + i * 2, redirect_lobyte);
  }
}

hal_irq_t init_apic(void) {
  // Set the spurious interrupt vecor
  write_apic_register(SPURIOUS_INTERRUPT_VECTOR_REG, 0xFF | (1 << 8));

  hal_irq_t ret;
  ret.eoi = apic_eoi;
  ret.map_irq = apic_map_irq;
  ret.mask_irq = apic_mask_irq;
  ret.unmask_irq = apic_unmask_irq;
  ret.mask_all_irqs = apic_mask_all_irqs;

  return ret;
}
