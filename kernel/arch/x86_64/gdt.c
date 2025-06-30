#include <decls.h>
#include <gdt.h>
#include <libk/kio.h>
#include <libk/mem.h>
#include <mem/memory.h>
#include <stddef.h>
#include <stdint.h>

ALIGN(0x8) gdt_descriptor_t gdt[DESCRIPTOR_COUNT];

gdt_pointer_t create_descriptors(void) {

  // Kernel Code
  gdt[1].access_byte = GDT_ACCESS_PRESENT | GDT_ACCESS_DESCRIPTOR |
                       GDT_ACCESS_EXECUTABLE | GDT_ACCESS_READ_WRITE;
  gdt[1].flags = GDT_LONG_MODE | GDT_GRANULARITY;
  gdt[1].limit_1 = 0xFFFF;
  gdt[1].limit_2 = 0xF;

  // Kernel Data
  gdt[2].access_byte =
      GDT_ACCESS_PRESENT | GDT_ACCESS_DESCRIPTOR | GDT_ACCESS_READ_WRITE;
  gdt[2].flags = GDT_GRANULARITY;
  gdt[2].limit_1 = 0xFFFF;
  gdt[2].limit_2 = 0xF;

  // User Code
  // (3 << 5 sets the DPL)
  gdt[3].access_byte = GDT_ACCESS_PRESENT | GDT_ACCESS_DESCRIPTOR |
                       GDT_ACCESS_EXECUTABLE | (3 << 5) | GDT_ACCESS_READ_WRITE;
  gdt[3].flags = GDT_LONG_MODE | GDT_GRANULARITY;
  gdt[3].limit_1 = 0xFFFF;
  gdt[3].limit_2 = 0xF;

  // User Data
  gdt[4].access_byte = GDT_ACCESS_PRESENT | GDT_ACCESS_DESCRIPTOR | (3 << 5) |
                       GDT_ACCESS_READ_WRITE;
  gdt[4].flags = GDT_LONG_MODE;
  gdt[4].limit_1 = 0xFFFF;
  gdt[4].limit_2 = 0xF;

  gdt_pointer_t ptr;
  // Add one because of the null descriptor
  ptr.size = sizeof(gdt_descriptor_t) * (DESCRIPTOR_COUNT + 1) - 1;
  ptr.offset = (size_t)gdt;

  return ptr;
}

extern void change_gdt(void);

void create_gdt(void) {
  gdt_pointer_t ptr = create_descriptors();

  // Load them here
  __asm__ volatile("lgdt (%0)" : : "r"(&ptr));

  change_gdt();

  // GDT no work without this
  kio_printf("");
}
