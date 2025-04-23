#include <acpi.h>
#include <arch/hal.h>
#include <asm.h>
#include <hal.h>
#include <stdint.h>

void pic_disable(void) {
  // I may be initializing the pic just to disable but imma do it anyways
  outb(0x20, 0x01 | 0x10);
  io_wait();
  outb(0xA0, 0x01 | 0x10);
  io_wait();
  outb(0x21, 32); // Map both pics to the same place
  io_wait();
  outb(0xA1, 32);
  io_wait();
  outb(0x21, 4);
  io_wait();
  outb(0xA1, 2);
  io_wait();

  outb(0x21, 0x01);
  io_wait();
  outb(0xA1, 0x01);
  io_wait();

  // Mask both pics
  outb(0x21, 0xff);
  outb(0xA1, 0xff);
}

/// Im pretty sure xapic is always available at least for what im doing but IDK
/// Also theres basically no security or anything so idgaf
bool xapic_available(void) {
  /// Im pretty sure xapic is always available but IDK
  return true;
}

void init_hal_interupts(void) {
  if (xapic_available()) {
    init_xapic();
  }
}

static hal_interupt interupt;

void init_xapic(void) {
  pic_disable();
  // Enable lapic

  MADT_t *madt = find_madt(find_xsdt(get_xsdp()));
  uint64_t lapic_ptr = (uint64_t)madt->lapic_addr;
  uint8_t *lapic_addr = (uint8_t *)(uint64_t)madt->lapic_addr;

  /// Enablie lapic
  wrmsr(0x1B, (((lapic_ptr >> 32) & 0x0f) & 0xffffffff00000000) |
                  ((lapic_ptr & 0xffff0000) | 0x800));

  // Set sppurioous interupt
  lapic_addr[0xF0] = 0xff;

  // I hope i did the above things right
  // Idk
}

hal_interupt get_hal_interupt(void) { return interupt; }
