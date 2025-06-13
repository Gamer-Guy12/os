#include <gdt.h>
#include <libk/mem.h>
#include <mem/memory.h>
#include <stddef.h>

gdt_pointer_t create_descriptors(void) {
  // Create a user and kernel code and a user and kernel data
  // 4 in total
#define DESCRIPTOR_COUNT 4
  gdt_descriptor_t *descriptors =
      kmalloc(sizeof(gdt_descriptor_t) * DESCRIPTOR_COUNT, 0);
  memset(descriptors, 0, sizeof(gdt_descriptor_t) * DESCRIPTOR_COUNT);

  // Kernel Code
  descriptors[0].access_byte =
      GDT_ACCESS_PRESENT | GDT_ACCESS_DESCRIPTOR | GDT_ACCESS_EXECUTABLE;
  descriptors[0].flags = GDT_LONG_MODE;

  // Kernel Data
  descriptors[1].access_byte = GDT_ACCESS_PRESENT | GDT_ACCESS_DESCRIPTOR;
  descriptors[1].flags = GDT_LONG_MODE;

  // User Code
  // (3 << 5 sets the DPL)
  descriptors[2].access_byte = GDT_ACCESS_PRESENT | GDT_ACCESS_DESCRIPTOR |
                               GDT_ACCESS_EXECUTABLE | (3 << 5);
  descriptors[2].flags = GDT_LONG_MODE;

  // User Data
  descriptors[3].access_byte =
      GDT_ACCESS_PRESENT | GDT_ACCESS_DESCRIPTOR | (3 << 5);
  descriptors[3].flags = GDT_LONG_MODE;

  gdt_pointer_t ptr;
  ptr.size = sizeof(gdt_descriptor_t) * DESCRIPTOR_COUNT - 1;
  ptr.offset = (size_t)descriptors;

  return ptr;
}

void create_gdt(void) {
  gdt_pointer_t ptr = create_descriptors();

  // Load them here
  __asm__ volatile("lgdt (%0)" : : "r"(&ptr));
}
