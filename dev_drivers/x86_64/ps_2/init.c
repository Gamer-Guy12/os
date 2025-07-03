#include <asm.h>
#include <libk/err.h>
#include <libk/kio.h>
#include <libk/sys.h>
#include <ps_2.h>
#include <stdbool.h>

bool initialized = false;

void init_ps_2_driver(void) {
  if (initialized) return;
  initialized = true;
  // Disable ports
  outb(PS_2_COMMAND_PORT, DISABLE_FIRST_PS_2_PORT);
  io_wait();
  wait_for_write();
  outb(PS_2_COMMAND_PORT, DISABLE_SECOND_PS_2_PORT);
  io_wait();

  // Flush output buffer
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

  // Self Test
  wait_for_write();
  outb(PS_2_COMMAND_PORT, TEST_PS_2_CONTROLLER);
  io_wait();
  wait_for_read();
  uint8_t result = inb(PS_2_DATA_PORT);

  if (result != TEST_PASSED) {
    kio_printf("Result %x\n", result);
    sys_panic(HAL_INIT_ERR | KBD_HANDLING_ERR);
  }

  // Test port 1
  wait_for_write();
  outb(PS_2_COMMAND_PORT, TEST_FIRST_PS_2_PORT);
  io_wait();
  wait_for_read();
  result = inb(PS_2_DATA_PORT);

  if (result != PORT_1_TEST_PASSED) {
    sys_panic(HAL_INIT_ERR | KBD_HANDLING_ERR);
  }

  // Enable First Port
  wait_for_write();
  outb(PS_2_COMMAND_PORT, ENABLE_FIRST_PS_2_PORT);
  io_wait();

  // Set Configuration Byte
  wait_for_write();
  outb(PS_2_COMMAND_PORT, READ_CONFIG_BYTE);
  io_wait();
  wait_for_read();
  config_byte = inb(PS_2_DATA_PORT);

  // Enable Port 1 IRQs
  config_byte |= 1;

  wait_for_write();
  outb(PS_2_COMMAND_PORT, WRITE_CONFIG_BYTE);
  io_wait();
  wait_for_write();
  outb(PS_2_DATA_PORT, config_byte);
  io_wait();

  // Reset 1st ps/2 port
  wait_for_write();
  outb(PS_2_DATA_PORT, RESET_PORT);

  // Wait for the 2 bytes
  bool AA = false;
  bool FA = false;

  wait_for_read();
  result = inb(PS_2_DATA_PORT);
  if (result == 0xAA)
    AA = true;
  else if (result == 0xFA)
    FA = true;
  else {
    kio_printf("Result is %x\n", result);
    sys_panic(HAL_INIT_ERR | KBD_HANDLING_ERR);
  }

  wait_for_read();
  result = inb(PS_2_DATA_PORT);
  if (result == 0xAA)
    AA = true;
  else if (result == 0xFA)
    FA = true;
  else {
    kio_printf("Result is %x\n", result);
    sys_panic(HAL_INIT_ERR | KBD_HANDLING_ERR);
  }

  if (!(FA && AA)) {
    sys_panic(KBD_HANDLING_ERR | HAL_INIT_ERR);
  }
}
