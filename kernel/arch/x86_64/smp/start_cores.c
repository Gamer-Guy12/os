#include <mem/pimemory.h>
#include <acpi/acpi.h>
#include <asm.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <x86_64.h>

extern uint32_t page_table_ptr;

void wait_ms(uint32_t ms) {
  for (size_t i = 0; i < ms; i++) {
    outb(0x43, 0x30);
    io_wait();
    outb(0x40, 0xA9);
    io_wait();
    outb(0x40, 0x4);
    io_wait();

    do {
      outb(0x43, 0xE2);
      io_wait();

      if (inb(0x40) & (1 << 7)) {
        break;
      }

      __asm__ volatile("pause" ::: "memory");

    } while (true);
  }
}

/// This one excludes unenableable cores
uint8_t get_cores(uint8_t *ids) {
  MADT_t *madt = acpi_get_struct("APIC");
  size_t size_left = madt->header.length - sizeof(MADT_t);
  uint8_t *ptr = (uint8_t*)(madt + 1);

  uint8_t core_count = 0;

  while (size_left > 0) {
    MADT_entry_header_t* header = (MADT_entry_header_t*)ptr;
    if (header->entry_type != 0) {
      if (size_left < header->record_length) {
        break;
      }
      size_left -= header->record_length;
      ptr += header->record_length;
      continue;
    }

    MADT_entry_0_t *entry = (MADT_entry_0_t*)header;
    ids[core_count] = entry->apic_id;
    core_count++;

    if (size_left < header->record_length) {
      break;
    }
    size_left -= header->record_length;
    ptr += header->record_length;
  }

  return core_count;
}

void start_cores(void) {
  /// This os supports a max of 32 cores
  uint8_t core_ids[32];
  uint8_t core_count = get_cores(core_ids);

  page_table_ptr = virt_to_phys(PML4_ADDR);

  core_ids[0] = core_count;
}
