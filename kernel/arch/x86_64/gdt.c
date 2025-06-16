#include "libk/kio.h"
#include <decls.h>
#include <gdt.h>
#include <libk/mem.h>
#include <mem/memory.h>
#include <stddef.h>
#include <stdint.h>

ALIGN(0x8) gdt_descriptor_t gdt[DESCRIPTOR_COUNT];

gdt_pointer_t create_descriptors(void) {

  // Kernel Code
  gdt[1].access_byte =
      GDT_ACCESS_PRESENT | GDT_ACCESS_DESCRIPTOR | GDT_ACCESS_EXECUTABLE;
  gdt[1].flags = GDT_LONG_MODE;

  // Kernel Data
  gdt[2].access_byte = GDT_ACCESS_PRESENT | GDT_ACCESS_DESCRIPTOR;
  gdt[2].flags = GDT_LONG_MODE;

  // User Code
  // (3 << 5 sets the DPL)
  gdt[3].access_byte = GDT_ACCESS_PRESENT | GDT_ACCESS_DESCRIPTOR |
                       GDT_ACCESS_EXECUTABLE | (3 << 5);
  gdt[3].flags = GDT_LONG_MODE;

  // User Data
  gdt[4].access_byte = GDT_ACCESS_PRESENT | GDT_ACCESS_DESCRIPTOR | (3 << 5);
  gdt[4].flags = GDT_LONG_MODE;

  gdt_pointer_t ptr;
  ptr.size = sizeof(gdt_descriptor_t) * DESCRIPTOR_COUNT - 1;
  ptr.offset = (size_t)gdt;

  union {
     gdt_descriptor_t descriptor;
     struct {
        uint64_t first;
        uint64_t second;
     };
  } dou;

  dou.descriptor = gdt[1];

  kio_printf("GDT[1] second %x, first %x\n", dou.second, dou.first);

  return ptr;
}

void create_gdt(void) {
  gdt_pointer_t ptr = create_descriptors();

  // Load them here
  __asm__ volatile("lgdt (%0)" : : "r"(&ptr));

  kio_printf("ptr.offset %x and size %x\n", ptr.offset, (size_t)ptr.size);
}
