#include <asm.h>
#include <libk/spinlock.h>
#include <ps_2.h>
#include <stddef.h>
#include <stdint.h>

uint8_t command_queue[COMMAND_QUEUE_LENGTH];
uint16_t read_index = 0;
uint16_t write_index = 0;
uint16_t command_count = 0;

spinlock_t command_queue_lock = 0;

bool executing = false;

void execute_queue(void) {
  while (command_count > 0) {
    uint8_t command = read_index;

    if (command == 0xEE) {
      for (size_t i = 0; i < 3; i++) {
        wait_for_write();
        outb(PS_2_DATA_PORT, command);
        io_wait();
        wait_for_read();
        uint8_t res = inb(PS_2_DATA_PORT);
        io_wait();
        if (res != RESEND) {
          break;
        }
      }
      continue;
    } else if (command == 0xFE) {
      for (size_t i = 0; i < 3; i++) {
        wait_for_write();
        outb(PS_2_DATA_PORT, command);
        io_wait();
        wait_for_read();
        uint8_t res = inb(PS_2_DATA_PORT);
        io_wait();
        if (res == ACK) {
          break;
        }
      }
      continue;
    }

    for (size_t i = 0; i < 3; i++) {
      wait_for_write();
      outb(PS_2_DATA_PORT, command);
      io_wait();
      wait_for_read();
      uint8_t res = inb(PS_2_DATA_PORT);
      io_wait();
      if (res == ACK) {
        break;
      }
    }

    read_index++;
    if (read_index == COMMAND_QUEUE_LENGTH) {
      read_index = 0;
    }
  }

  executing = false;
}

bool enqueue_command(uint8_t command) {
  spinlock_acquire(&command_queue_lock);

  if (command_count == COMMAND_QUEUE_LENGTH) {
    return false;
    spinlock_release(&command_queue_lock);
  }

  command_queue[write_index] = command;

  command_count++;
  write_index++;
  if (write_index == COMMAND_QUEUE_LENGTH) {
    write_index = 0;
  }

  spinlock_release(&command_queue_lock);

  if (!executing) {
    executing = true;
    execute_queue();
  }

  return true;
}
