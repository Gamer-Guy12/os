#include <libk/key.h>
#include <stdint.h>

static uint8_t translation_table[] = {
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0, 0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0, 0,
    '~', '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', 0,   0, 0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0, 0,
    0,   'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '|', 0, 0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0, 0,
    0,   'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '"', 0,   0,   0, 0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0, 0,
    0,   'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?', 0,   0,   0,   0, 0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0, 0,
    0,   0,   0,   ' ', 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0, 0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0, 0};

uint8_t shift_keycode_to_char(uint8_t keycode) {
  return translation_table[keycode];
}
