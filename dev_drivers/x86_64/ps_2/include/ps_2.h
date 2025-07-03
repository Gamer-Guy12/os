#ifndef X86_64_PS_2
#define X86_64_PS_2

#include <stdint.h>
#include <stdbool.h>

#define PS_2_DATA_PORT 0x60
#define PS_2_STATUS_PORT 0x64
#define PS_2_COMMAND_PORT 0x64

#define DISABLE_FIRST_PS_2_PORT 0xAD
#define DISABLE_SECOND_PS_2_PORT 0xA7
#define READ_CONFIG_BYTE 0x20
#define WRITE_CONFIG_BYTE 0x60
#define TEST_PS_2_CONTROLLER 0xAA
#define TEST_FIRST_PS_2_PORT 0xAB
#define ENABLE_FIRST_PS_2_PORT 0xAE
#define RESET_PORT 0xFF
#define PS_2_ECHO 0xEE
#define PS_2_RESEND 0xFE

#define TEST_PASSED 0x55
#define PORT_1_TEST_PASSED 0x0

#define ACK 0xFA
#define RESEND 0xFE

#define COMMAND_QUEUE_LENGTH 512

typedef enum {
  KEY_STATE_UP = 0,
  KEY_STATE_DOWN = 1
} key_state_t;

void init_ps_2_driver(void);
void wait_for_write(void);
void wait_for_read(void);

void set_key_state(uint8_t key_code, uint8_t state);
uint8_t get_key_state(uint8_t key_code);

bool enqueue_command(uint8_t command);

typedef enum {
  KEY_RELEASED = 1,
} key_press_flags_t;

// Reads it into the key states
uint16_t handle_key_press(void);

void set_caps_key(bool is_on);
bool get_caps_key_on(void);

#endif

