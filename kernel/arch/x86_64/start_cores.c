#include <acpi.h>
#include <io.h>
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

void start_cores(uint8_t *multiboot) {
  lock_t starting_cores;

  lock_acquire(&starting_cores);

  XSDP_t *xsdp = find_xsdp(multiboot);
  XSDT_t *xsdt = find_xsdt(xsdp);
  MADT_t *madt = find_madt(xsdt);
  uint8_t core_count = get_cores(NULL, madt);
  uint8_t cores[core_count];
  get_cores(cores, madt);
  volatile uint32_t *lapic_addr = (uint32_t *)(uint64_t)madt->lapic_addr;

  kio_printf("The lapic addr is %u\n", (uint64_t)lapic_addr);

  uint8_t bspid = 0;
  __asm__ __volatile__("mov $1, %%eax; cpuid; shrl $24, %%ebx;"
                       : "=b"(bspid)
                       :
                       :);

  kio_printf("The bspid is %u", (uint64_t)bspid);

  wait_ms(1000);

  // memcpy(&core_count, (void *)0x8000, 1);
  memcpy(&ap_tramp, (void *)0x8000, ap_len);

  wait_ms(1000);

  // for (size_t i = 0; i < core_count; i++) {
  //   if (cores[i] == bspid)
  //     continue;
  //
  //   lapic_addr[0x280] = 0;
  //   lapic_addr[0x310] = (lapic_addr[0x310] & 0x00FFFFFF) | cores[i] << 24;
  //   lapic_addr[0x300] = 0x00004500;
  //   wait_ms(1000);
  //   do {
  //     __asm__ __volatile__("pause" : : : "memory");
  //   } while (!(lapic_addr[0x300] & 1 << 12));
  //   wait_ms(1000);
  //
  //   for (size_t j = 0; j < 2; j++) {
  //     lapic_addr[0x280] = 0;
  //     lapic_addr[0x310] = (lapic_addr[0x310] & 0x00FFFFFF) | cores[i] << 24;
  //     lapic_addr[0x300] = 0x00004600 | 8;
  //     wait_ms(1);
  //     do {
  //       __asm__ __volatile__("pause" : : : "memory");
  //     } while (!(lapic_addr[0x300] & 1 << 12));
  //   }
  // }

  for (size_t i = 0; i < core_count; i++) {
    // do not start BSP, that's already running this code
    if (cores[i] == bspid)
      continue;
    // send INIT IPI
    *((volatile uint32_t *)(lapic_addr + 0x280)) = 0; // clear APIC errors
    *((volatile uint32_t *)(lapic_addr + 0x310)) =
        (*((volatile uint32_t *)(lapic_addr + 0x310)) & 0x00ffffff) |
        (i << 24); // select AP
    *((volatile uint32_t *)(lapic_addr + 0x300)) =
        (*((volatile uint32_t *)(lapic_addr + 0x300)) & 0xfff00000) |
        0x00C500; // trigger INIT IPI
    do {
      __asm__ __volatile__("pause" : : : "memory");
    } while (*((volatile uint32_t *)(lapic_addr + 0x300)) &
             (1 << 12)); // wait for delivery
    *((volatile uint32_t *)(lapic_addr + 0x310)) =
        (*((volatile uint32_t *)(lapic_addr + 0x310)) & 0x00ffffff) |
        (i << 24); // select AP
    *((volatile uint32_t *)(lapic_addr + 0x300)) =
        (*((volatile uint32_t *)(lapic_addr + 0x300)) & 0xfff00000) |
        0x008500; // deassert
    do {
      __asm__ __volatile__("pause" : : : "memory");
    } while (*((volatile uint32_t *)(lapic_addr + 0x300)) &
             (1 << 12)); // wait for delivery
    wait_ms(10); // wait 10 msec
    // send STARTUP IPI (twice)
    for (size_t j = 0; j < 2; j++) {
      *((volatile uint32_t *)(lapic_addr + 0x280)) = 0; // clear APIC errors
      *((volatile uint32_t *)(lapic_addr + 0x310)) =
          (*((volatile uint32_t *)(lapic_addr + 0x310)) & 0x00ffffff) |
          (i << 24); // select AP
      *((volatile uint32_t *)(lapic_addr + 0x300)) =
          (*((volatile uint32_t *)(lapic_addr + 0x300)) & 0xfff0f800) |
          0x000608; // trigger STARTUP IPI for 0800:0000
      wait_ms(1);   // wait 200 usec
      do {
        __asm__ __volatile__("pause" : : : "memory");
      } while (*((volatile uint32_t *)(lapic_addr + 0x300)) &
               (1 << 12)); // wait for delivery
    }
  }

  lock_release(&starting_cores);
}
