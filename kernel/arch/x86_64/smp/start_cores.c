#include <acpi/acpi.h>
#include <apic.h>
#include <asm.h>
#include <libk/kio.h>
#include <libk/math.h>
#include <libk/mem.h>
#include <mem/memory.h>
#include <mem/pimemory.h>
#include <mem/vimemory.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <x86_64.h>

extern uint32_t page_table_ptr;

extern char _smp_code[];
extern char _smp_data_end[];

// Max core count is 256
size_t stack_ptrs[256];

volatile uint8_t ap_running = 0;
uint32_t bspid = 0;
extern uint32_t bspdone;

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

void wait_us(uint32_t us) {
  outb(0x43, 0x30);
  io_wait();
#define CLOCK_FREQUENCY 1193182
  uint16_t value = CLOCK_FREQUENCY / (1000000 / us);

  outb(0x40, value & 0xff);
  io_wait();
  outb(0x40, value >> 8);
  io_wait();

  do {
    outb(0x43, 0xE2);
    io_wait();

    if (inb(0x40) & (1 << 7)) {
      break;
    }
    io_wait();

    __asm__ volatile("pause" ::: "memory");
  } while (true);
}

/// This one excludes unenableable cores
uint8_t get_cores(uint8_t *ids) {
  MADT_t *madt = acpi_get_struct("APIC");
  size_t size_left = madt->header.length - sizeof(MADT_t);
  uint8_t *ptr = (uint8_t *)(madt + 1);

  uint8_t core_count = 0;

  while (size_left > 0) {
    MADT_entry_header_t *header = (MADT_entry_header_t *)ptr;
    if (header->entry_type != 0) {
      if (size_left < header->record_length) {
        break;
      }
      size_left -= header->record_length;
      ptr += header->record_length;
      continue;
    }

    MADT_entry_0_t *entry = (MADT_entry_0_t *)header;
    if (ids != NULL)
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

void wait_for_delivery(void) {
  do {
    __asm__ volatile("pause" ::: "memory");
  } while (read_apic_register(INTERRUPT_COMMAND_REG_1) & (1 << 12));
}

void start_cores(void) {
  uint8_t core_count = get_cores(NULL);
  uint8_t core_ids[core_count];
  get_cores(core_ids);

  *((uint32_t *)((size_t)&page_table_ptr + IDENTITY_MAPPED_ADDR)) =
      virt_to_phys(PML4_ADDR);

  uint8_t *smp_code = (void *)_smp_code;
  uint8_t *smp_data_end = (void *)_smp_data_end;

  size_t length = smp_data_end - smp_code;
  memcpy((void *)((size_t)smp_code + IDENTITY_MAPPED_ADDR),
         (void *)(0x8000 + IDENTITY_MAPPED_ADDR), length);

  // Map the smp startup trampoline into virtual memory (all processors
  // currently still use the same tables)
  size_t page_count = ROUND_UP(length, PAGE_SIZE) / PAGE_SIZE;
  for (size_t i = 0; i < page_count; i++) {
    map_phys_page((void *)(0x8000 + i * PAGE_SIZE), PT_PRESENT | PT_READ_WRITE,
                  0, (void *)(0x8000 + i * PAGE_SIZE));
  }

  __asm__ volatile("mov $1, %%eax; cpuid; shrl $24, %%ebx;"
                   : "=b"(bspid)::"rax");

  // -1 so we don't allocate for ourselves
  for (size_t i = 0; i < core_count; i++) {
    kio_printf("Core Id: %x\n", core_ids[i]);
    if (i == bspid)
      continue;

    size_t ptr = (size_t)phys_alloc();
    // Stack grows down so we offset to the top
    stack_ptrs[core_ids[i]] = ptr + IDENTITY_MAPPED_ADDR + PAGE_SIZE - 1;
  }

  bspdone = 0;

  for (size_t i = 0; i < core_count; i++) {
    if (core_ids[i] == bspid) {
      continue;
    }

    // Clear Errors
    write_apic_register(ERROR_STATUS_REG, 0);

    // Set destination apic
    uint32_t save_register = read_apic_register(INTERRUPT_COMMAND_REG_2);
    save_register = save_register & 0x00ffffff;
    save_register |= (core_ids[i] << 24);
    write_apic_register(INTERRUPT_COMMAND_REG_2, save_register);

    // Issue init ipi
    save_register = read_apic_register(INTERRUPT_COMMAND_REG_1);
    save_register = save_register & 0xfff00000;
    save_register |= 0x00C500;
    write_apic_register(INTERRUPT_COMMAND_REG_1, save_register);

    wait_for_delivery();

    // Set destination apic
    save_register = read_apic_register(INTERRUPT_COMMAND_REG_2);
    save_register = save_register & 0x00ffffff;
    save_register |= (core_ids[i] << 24);
    write_apic_register(INTERRUPT_COMMAND_REG_2, save_register);

    // Deassert init ipi
    save_register = read_apic_register(INTERRUPT_COMMAND_REG_1);
    save_register = save_register & 0xfff00000;
    save_register |= 0x008500;
    write_apic_register(INTERRUPT_COMMAND_REG_1, save_register);

    wait_for_delivery();

    wait_ms(10);

    for (size_t j = 0; j < 2; j++) {
      // Clear errors
      write_apic_register(ERROR_STATUS_REG, 0);

      // Set destination apic
      save_register = read_apic_register(INTERRUPT_COMMAND_REG_2);
      save_register = save_register & 0x00ffffff;
      save_register |= (core_ids[i] << 24);
      write_apic_register(INTERRUPT_COMMAND_REG_2, save_register);

      // Issue startup ipi
      save_register = read_apic_register(INTERRUPT_COMMAND_REG_1);
      save_register = save_register * 0xfff00000;
      save_register |= (8 | 6 << 8);
      write_apic_register(INTERRUPT_COMMAND_REG_1, save_register);

      wait_us(200);

      wait_for_delivery();
    }
  }

  bspdone = 1;

  while (ap_running != core_count - 1) {
    __asm__ volatile("pause" ::: "memory");
  }

  // Unmap what was mapped
  for (size_t i = 0; i < page_count; i++) {
    unmap_page((void *)(0x8000 + i * PAGE_SIZE), false);
  }
}
