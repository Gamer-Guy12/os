#include "acpi.h"
#include <acpi.h>
#include <libk/kgfx.h>
#include <libk/kio.h>
#include <libk/lock.h>
#include <libk/string.h>
#include <pmm.h>
#include <stddef.h>
#include <stdint.h>
#include <x86_64.h>

extern void kernel_main(void);

// Code thanks to nullplan from the osdev wiki
// https://forum.osdev.org/viewtopic.php?t=57103 This code calls the global
// constructors
#pragma region Global Constructors
typedef void initfunc_t(void);
extern initfunc_t *__init_array_start[], *__init_array_end[];

__attribute__((section(".startup"))) static void handle_init_array(void) {
  size_t nfunc = ((uintptr_t)__init_array_end - (uintptr_t)__init_array_start) /
                 sizeof(initfunc_t *);
  for (initfunc_t **p = __init_array_start; p < __init_array_start + nfunc; p++)
    (*p)();
}
#pragma endregion

void print_multiboot_size(uint8_t *multiboot) {
  uint64_t size = *((uint32_t *)multiboot);
  uint64_t reserved = ((uint32_t *)multiboot)[1];

  kio_printf(
      "The size of the multiboot header is: %x or %u and the reserved is %x\n",
      size, size, reserved);
}

void print_xsdp(uint8_t *multiboot) {
  XSDP_t *xsdp = find_xsdp(multiboot);
  kio_printf("The XSDP:\n");
  kio_printf("Signature: ");
  for (size_t i = 0; i < 8; i++) {
    kio_putchar(xsdp->signature[i]);
  }
  kio_putchar('\n');
  kio_printf("Checksum: %u\n", (uint64_t)xsdp->checksum);
  kio_printf("OEMID: ");
  for (size_t i = 0; i < 6; i++) {
    kio_putchar(xsdp->OEMID[i]);
  }
  kio_putchar('\n');
  kio_printf("Revision: %u\n", (uint64_t)xsdp->revision);
  kio_printf("RSDT Addr: %u\n", (uint64_t)xsdp->rsdt_addr);
  kio_printf("Length: %u\n", (uint64_t)xsdp->length);
  kio_printf("XSDT Addr: %u\n", (uint64_t)xsdp->xsdt_addr);
  kio_printf("Extended Checksum: %u\n", (uint64_t)xsdp->extended_checksum);
  kio_printf("Reserved: ");
  for (size_t i = 0; i < 3; i++) {
    kio_printf("%u", (uint64_t)xsdp->reserved[i]);
  }
  kio_printf("\n");
}

__attribute__((section(".startup"))) void kernel_start(uint8_t *multiboot) {
  handle_init_array();
  kgfx_clear();
  print_multiboot_size(multiboot);
  setup_page_frame_allocation(multiboot);
  print_xsdp(multiboot);
  // find_xsdp(multiboot);

  XSDP_t *xsdp = find_xsdp(multiboot);
  kio_printf("Got XSDP\n");
  XSDT_t *xsdt = find_xsdt(xsdp);
  kio_printf("Got XSDT\n");
  MADT_t *madt = find_madt(xsdt);
  kio_printf("Got MADT\n");
  madt += madt - madt;
  uint16_t core_count = get_cores(NULL, madt);
  kio_printf("Got Cores\n");
  kio_printf("%u\n", core_count);
  start_cores(multiboot);
  // kernel_main();
}
