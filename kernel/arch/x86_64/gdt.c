#include <gdt.h>
#include <libk/mem.h>
#include <mem/memory.h>
#include <stddef.h>

gdt_descriptor_t gdt[DESCRIPTOR_COUNT];

gdt_pointer_t create_descriptors(void) {

  // Kernel Code
  gdt[0].access_byte =
      GDT_ACCESS_PRESENT | GDT_ACCESS_DESCRIPTOR | GDT_ACCESS_EXECUTABLE;
  gdt[0].flags = GDT_LONG_MODE;

  // Kernel Data
  gdt[1].access_byte = GDT_ACCESS_PRESENT | GDT_ACCESS_DESCRIPTOR;
  gdt[1].flags = GDT_LONG_MODE;

  // User Code
  // (3 << 5 sets the DPL)
  gdt[2].access_byte = GDT_ACCESS_PRESENT | GDT_ACCESS_DESCRIPTOR |
                               GDT_ACCESS_EXECUTABLE | (3 << 5);
  gdt[2].flags = GDT_LONG_MODE;

  // User Data
  gdt[3].access_byte =
      GDT_ACCESS_PRESENT | GDT_ACCESS_DESCRIPTOR | (3 << 5);
  gdt[3].flags = GDT_LONG_MODE;

  gdt_pointer_t ptr;
  ptr.size = sizeof(gdt_descriptor_t) * DESCRIPTOR_COUNT - 1;
  ptr.offset = (size_t)gdt;

  return ptr;
}

void create_gdt(void) {
  gdt_pointer_t ptr = create_descriptors();

  // Load them here
  __asm__ volatile("lgdt (%0)" : : "r"(&ptr));
}
