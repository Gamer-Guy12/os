#include <cls.h>
#include <decls.h>
#include <gdt.h>
#include <libk/kio.h>
#include <libk/mem.h>
#include <mem/memory.h>
#include <stddef.h>
#include <stdint.h>

tss_t *create_tss(void) {
  tss_t *tss = gmalloc(sizeof(tss_t));

  return tss;
}

gdt_pointer_t create_descriptors(void) {

  gdt_descriptor_t *gdt = get_cls()->gdt;

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

  // TSS
  tss_t *tss = create_tss();
  cls_t *cls = get_cls();
  cls->tss = tss;

  gdt_system_segment_t segment;
  segment.flags = GDT_LONG_MODE;
  segment.access_byte = GDT_SYSTEM_TSS_AVAILABLE | GDT_ACCESS_PRESENT;

  size_t tss_ptr = (size_t)tss;
  segment.base_0 = tss_ptr & 0xfff;
  segment.base_1 = (tss_ptr >> 16) & 0xff;
  segment.base_2 = (tss_ptr >> 24) & 0xff;
  segment.base_3 = (tss_ptr >> 32) & 0xffffffff;

  union {
    gdt_system_segment_t segment;
    struct {
      gdt_descriptor_t lobyte;
      gdt_descriptor_t hibyte;
    };
  } tss_union;

  tss_union.segment = segment;

  gdt[5] = tss_union.lobyte;
  gdt[6] = tss_union.hibyte;

  // Load the tss
  __asm__ volatile("ltr %%ax" ::"a"(0x28));

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
