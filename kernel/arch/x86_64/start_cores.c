/// A lot of code in this file is from osdev and their SMP page
/// Full credit to them for so much help
/// https://wiki.osdev.org/Symmetric_Multiprocessing

#include <acpi.h>
#include <asm.h>
#include <libk/bit.h>
#include <libk/kio.h>
#include <libk/lock.h>
#include <libk/mem.h>
#include <stddef.h>
#include <stdint.h>
#include <x86_64.h>

extern uint16_t ap_tramp;
extern uint16_t ap_len;
volatile uint8_t aps_running = 0;

static inline void wait_ms(uint32_t ms) {
  for (size_t i = 0; i < ms; i++) {
    outb(0x43, 0x30);
    io_wait();
    outb(0x40, 0xA9);
    io_wait();
    outb(0x40, 0x4);
    io_wait();

    do {
      outb(0x43, 0xE2);
    } while (!check_bit(inb(0x40), 7));
  }
}

static inline void wait_us(uint32_t us) {
  for (size_t i = 0; i < us; i++) {
    outb(0x43, 0x30);
    io_wait();
    outb(0x40, 0x1);
    io_wait();
    outb(0x40, 0x0);
    io_wait();

    do {
      outb(0x43, 0xE2);
    } while (!check_bit(inb(0x40), 7));
  }
}

static lock_t starting_cores;

/// Pause time is in micro seconds
/// A lot of this stuff is just for starting cores but im putting it here
void issue_ipi(uint8_t core, uint8_t ipi_type, bool deassert,
               uint16_t pause_time, uint32_t or_with) {
  XSDP_t *xsdp = get_xsdp();
  XSDT_t *xsdt = find_xsdt(xsdp);
  MADT_t *madt = find_madt(xsdt);

  uint8_t *lapic_addr = (uint8_t *)(uint64_t)madt->lapic_addr;

  // Clear errors
  lapic_addr[0x280] = 0;
  // Set destination
  lapic_addr[0x310] = core << 24;
  // Set info into 0x300 ICR
  lapic_addr[0x300] =
      /* Preserve reserved */ (lapic_addr[0x300] & 0xfff00000) | (1 << 14) |
      ((ipi_type & 0x3) << 8) | or_with;
  do {
    __asm__ volatile("pause" : : : "memory");
  } while (lapic_addr[0x300] & (1 << 12));

  if (pause_time > 0)
    wait_us(pause_time);

  if (deassert) {
    lapic_addr[0x280] = 0;
    lapic_addr[0x310] = core << 24;
    lapic_addr[0x300] =
        /* Preserve reserved */ (lapic_addr[0x300] & 0xfff00000) | (0 << 14) |
        ((ipi_type & 0x3) << 8);

    do {
      __asm__ volatile("pause" : : : "memory");
    } while (lapic_addr[0x300] & (1 << 12));
  }
}

void start_cores(void) {

  lock_acquire(&starting_cores);

  kio_printf("Starting to initialize core\n");

  // Pause a second to stop clangd from complaining about uncalled function
  // wait_ms(1000);
  wait_us(1000);

  // Copy ap trampoline
  memcpy(&ap_tramp, (void *)0x8000, ap_len);

  // Get the id of the current core so that we don't start it again
  uint8_t bspid;
  __asm__ volatile("mov $1, %%eax; cpuid; shrl $24, %%ebx;" : "=b"(bspid) : :);

  XSDP_t *xsdp = get_xsdp();
  XSDT_t *xsdt = find_xsdt(xsdp);
  MADT_t *madt = find_madt(xsdt);
  size_t core_count = get_cores(NULL, madt);
  uint8_t cores[core_count];
  get_cores(cores, madt);

  for (size_t i = 0; i < core_count; i++) {

    kio_printf("Initializing core %u\n", i);
    // Skip bsp
    if (cores[i] == bspid)
      continue;

    // IPI Type 101 is an INIT IPI
    issue_ipi(cores[i], 101, true, 0, 8);
    wait_ms(10);

    for (size_t j = 0; j < 2; j++) {
      // IPI Type 110 is startup
      issue_ipi(cores[i], 110, false, 200, 0);
    }
  }

  lock_release(&starting_cores);
}
