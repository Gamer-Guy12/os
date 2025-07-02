#include <asm.h>
#include <hal/kbd.h>
#include <hal/x86_64_kbd.h>

#define PS_2_DATA_PORT 0x60
#define PS_2_STATUS_PORT 0x64
#define PS_2_COMMAND_PORT 0x64

#define DISABLE_FIRST_PS_2_PORT 0xAD
#define DISABLE_SECOND_PS_2_PORT 0xA7
#define READ_CONFIG_BYTE 0x20
#define WRITE_CONFIG_BYTE 0x60

bool check_ps_2(void) {
  // IMPORTANT: TODO: CHECK FOR PS/2 IN FADT (ACPI TABLE WITH SIGNATURE 'FACP')

  return true;
}

static void wait_for_read(void) {
  while (!(inb(PS_2_STATUS_PORT) & 1)) {
  }
}

static void wait_for_write(void) {
  while (inb(PS_2_STATUS_PORT) & 2) {
  }
}

static void init(void) {
  // Disable ports
  outb(PS_2_COMMAND_PORT, DISABLE_FIRST_PS_2_PORT);
  io_wait();
  wait_for_write();
  outb(PS_2_COMMAND_PORT, DISABLE_SECOND_PS_2_PORT);
  io_wait();

  // Flush output buffer
  wait_for_read();
  inb(PS_2_DATA_PORT);
  io_wait();

  // Set Configuration Byte
  wait_for_write();
  outb(PS_2_COMMAND_PORT, READ_CONFIG_BYTE);
  io_wait();
  wait_for_read();
  uint8_t config_byte = inb(PS_2_DATA_PORT);

  // Disable translation
  config_byte &= ~(1 << 6);

  wait_for_write();
  outb(PS_2_COMMAND_PORT, WRITE_CONFIG_BYTE);
  io_wait();
  wait_for_write();
  outb(PS_2_DATA_PORT, config_byte);
  io_wait();
}

hal_kbd_t init_ps_2(void) { init(); }
