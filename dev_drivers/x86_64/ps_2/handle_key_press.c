#include <asm.h>
#include <hal/kbd.h>
#include <ps_2.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

// The index is the scancode and the value is the key code
uint8_t translation_table[] = {
    KEY_NONE,
    QWERTY_KEY_F9,
    KEY_NONE,
    QWERTY_KEY_F5,
    QWERTY_KEY_F3,
    QWERTY_KEY_F1,
    QWERTY_KEY_F2,
    QWERTY_KEY_F12,
    KEY_NONE,
    QWERTY_KEY_F10,
    QWERTY_KEY_F8,
    QWERTY_KEY_F6,
    QWERTY_KEY_F4,
    QWERTY_KEY_TAB,
    QWERTY_KEY_GRAVE,
    KEY_NONE,
    KEY_NONE,
    QWERTY_KEY_L_ALT,
    QWERTY_KEY_L_SHIFT,
    KEY_NONE,
    QWERTY_KEY_L_CTRL,
    QWERTY_KEY_Q,
    QWERTY_KEY_1,
    KEY_NONE,
    KEY_NONE,
    KEY_NONE,
    QWERTY_KEY_Z,
    QWERTY_KEY_S,
    QWERTY_KEY_A,
    QWERTY_KEY_W,
    QWERTY_KEY_2,
    KEY_NONE,
    KEY_NONE,
    QWERTY_KEY_C,
    QWERTY_KEY_X,
    QWERTY_KEY_D,
    QWERTY_KEY_E,
    QWERTY_KEY_4,
    QWERTY_KEY_3,
    KEY_NONE,
    KEY_NONE,
    QWERTY_KEY_SPACE,
    QWERTY_KEY_V,
    QWERTY_KEY_F,
    QWERTY_KEY_T,
    QWERTY_KEY_R,
    QWERTY_KEY_5,
    KEY_NONE,
    KEY_NONE,
    QWERTY_KEY_N,
    QWERTY_KEY_B,
    QWERTY_KEY_H,
    QWERTY_KEY_G,
    QWERTY_KEY_Y,
    QWERTY_KEY_6,
    KEY_NONE,
    KEY_NONE,
    KEY_NONE,
    QWERTY_KEY_M,
    QWERTY_KEY_J,
    QWERTY_KEY_U,
    QWERTY_KEY_7,
    QWERTY_KEY_8,
    KEY_NONE,
    KEY_NONE,
    QWERTY_KEY_COMMA,
    QWERTY_KEY_K,
    QWERTY_KEY_I,
    QWERTY_KEY_O,
    QWERTY_KEY_0,
    QWERTY_KEY_9,
    KEY_NONE,
    KEY_NONE,
    QWERTY_KEY_PERIOD,
    QWERTY_KEY_SLASH,
    QWERTY_KEY_L,
    QWERTY_KEY_SEMICOLON,
    QWERTY_KEY_P,
    QWERTY_KEY_DASH,
    KEY_NONE,
    KEY_NONE,
    KEY_NONE,
    QWERTY_KEY_QUOTE,
    KEY_NONE,
    QWERTY_KEY_OPEN_BRACKET,
    QWERTY_KEY_EQUALS,
    KEY_NONE,
    KEY_NONE,
    QWERTY_KEY_CAPS,
    QWERTY_KEY_R_SHIFT,
    QWERTY_KEY_ENTER,
    QWERTY_KEY_CLOSE_BRACKET,
    KEY_NONE,
    QWERTY_KEY_BACK_SLASH,
    KEY_NONE,
    KEY_NONE,
    KEY_NONE,
    KEY_NONE,
    KEY_NONE,
    KEY_NONE,
    KEY_NONE,
    KEY_NONE,
    QWERTY_KEY_BACKSPACE,
    KEY_NONE,
    KEY_NONE,
    KEY_NONE,
    KEY_NONE,
    KEY_NONE,
    KEY_NONE,
    KEY_NONE,
    KEY_NONE,
    KEY_NONE,
    KEY_NONE,
    KEY_NONE,
    KEY_NONE,
    KEY_NONE,
    KEY_NONE,
    KEY_NONE,
    QWERTY_KEY_ESC,
    KEY_NONE,
    QWERTY_KEY_F11,
    KEY_NONE,
    KEY_NONE,
    KEY_NONE,
    KEY_NONE,
    KEY_NONE,
    KEY_NONE,
    KEY_NONE,
    KEY_NONE,
    KEY_NONE,
    KEY_NONE,
    QWERTY_KEY_F7,
};

// Same indexing as previous one
// TODO: IMPORTANT: ADD IN RIGHT CONTROL AND RIGHT ALT CUZ THEY IN HERE
uint8_t extended_translation_table[] = {KEY_NONE};

void handle_key_press(void) {
  bool prefix_state = false;
  bool release_state = false;

  while (true) {
    wait_for_read();
    uint8_t byte = inb(PS_2_DATA_PORT);

    // Handle pause key
    if (byte == 0xE1) {
      // Finish out the next 7 bytes then break
      for (size_t i = 0; i < 7; i++) {
        wait_for_read();
        inb(PS_2_DATA_PORT);
      }

      break;
    }

    // Handle states
    if (byte == 0xE0) {
      prefix_state = true;
      continue;
    } else if (byte == 0xF0) {
      release_state = true;
      continue;
    }

    // Handle prnt screen
    if (prefix_state && byte == 0x12 && !release_state) {
      wait_for_read();
      inb(PS_2_DATA_PORT);

      wait_for_read();
      inb(PS_2_DATA_PORT);

      break;

    } else if (prefix_state && byte == 0x7C && release_state) {
      wait_for_read();
      inb(PS_2_DATA_PORT);

      wait_for_read();
      inb(PS_2_DATA_PORT);

      wait_for_read();
      inb(PS_2_DATA_PORT);

      break;
    }

    // Translate and write
    uint8_t keycode = 0x0;

    if (prefix_state) {
      keycode = extended_translation_table[byte];
    } else {
      keycode = translation_table[byte];
    }

    if (keycode == KEY_NONE) {
      break;
    }

    set_key_state(keycode, release_state ? KEY_STATE_UP : KEY_STATE_DOWN);

    // Handle caps
    if (keycode == QWERTY_KEY_CAPS && release_state) {
      bool cur_caps = get_caps_key_on();
      set_caps_key(!cur_caps);
    }

    break;
  }
}
