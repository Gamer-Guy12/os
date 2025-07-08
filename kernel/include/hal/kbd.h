#ifndef HAL_KBD_H
#define HAL_KBD_H

#include <stdbool.h>
#include <stdint.h>

// Keycodes
#define QWERTY_KEY_ESC 0x0
#define QWERTY_KEY_F1 0x2
#define QWERTY_KEY_F2 0x3
#define QWERTY_KEY_F3 0x4
#define QWERTY_KEY_F4 0x5
#define QWERTY_KEY_F5 0x6
#define QWERTY_KEY_F6 0x7
#define QWERTY_KEY_F7 0x8
#define QWERTY_KEY_F8 0x9
#define QWERTY_KEY_F9 0xA
#define QWERTY_KEY_F10 0xB
#define QWERTY_KEY_F11 0xC
#define QWERTY_KEY_F12 0xD
#define QWERTY_KEY_GRAVE (1 << 5)
#define QWERTY_KEY_1 ((1 << 5) | 0x1)
#define QWERTY_KEY_2 ((1 << 5) | 0x2)
#define QWERTY_KEY_3 ((1 << 5) | 0x3)
#define QWERTY_KEY_4 ((1 << 5) | 0x4)
#define QWERTY_KEY_5 ((1 << 5) | 0x5)
#define QWERTY_KEY_6 ((1 << 5) | 0x6)
#define QWERTY_KEY_7 ((1 << 5) | 0x7)
#define QWERTY_KEY_8 ((1 << 5) | 0x8)
#define QWERTY_KEY_9 ((1 << 5) | 0x9)
#define QWERTY_KEY_0 ((1 << 5) | 0xA)
#define QWERTY_KEY_DASH ((1 << 5) | 0xB)
#define QWERTY_KEY_EQUALS ((1 << 5) | 0xC)
#define QWERTY_KEY_BACKSPACE ((1 << 5) | 0xD)
#define QWERTY_KEY_TAB (2 << 5)
#define QWERTY_KEY_Q ((2 << 5) | 0x1)
#define QWERTY_KEY_W ((2 << 5) | 0x2)
#define QWERTY_KEY_E ((2 << 5) | 0x3)
#define QWERTY_KEY_R ((2 << 5) | 0x4)
#define QWERTY_KEY_T ((2 << 5) | 0x5)
#define QWERTY_KEY_Y ((2 << 5) | 0x6)
#define QWERTY_KEY_U ((2 << 5) | 0x7)
#define QWERTY_KEY_I ((2 << 5) | 0x8)
#define QWERTY_KEY_O ((2 << 5) | 0x9)
#define QWERTY_KEY_P ((2 << 5) | 0xA)
#define QWERTY_KEY_OPEN_BRACKET ((2 << 5) | 0xB)
#define QWERTY_KEY_CLOSE_BRACKET ((2 << 5) | 0xC)
#define QWERTY_KEY_BACK_SLASH ((2 << 5) | 0xD)
#define QWERTY_KEY_CAPS (3 << 5)
#define QWERTY_KEY_A ((3 << 5) | 0x1)
#define QWERTY_KEY_S ((3 << 5) | 0x2)
#define QWERTY_KEY_D ((3 << 5) | 0x3)
#define QWERTY_KEY_F ((3 << 5) | 0x4)
#define QWERTY_KEY_G ((3 << 5) | 0x5)
#define QWERTY_KEY_H ((3 << 5) | 0x6)
#define QWERTY_KEY_J ((3 << 5) | 0x7)
#define QWERTY_KEY_K ((3 << 5) | 0x8)
#define QWERTY_KEY_L ((3 << 5) | 0x9)
#define QWERTY_KEY_SEMICOLON ((3 << 5) | 0xA)
#define QWERTY_KEY_QUOTE ((3 << 5) | 0xB)
#define QWERTY_KEY_ENTER ((3 << 5) | 0xC)
#define QWERTY_KEY_L_SHIFT (4 << 5)
#define QWERTY_KEY_Z ((4 << 5) | 0x1)
#define QWERTY_KEY_X ((4 << 5) | 0x2)
#define QWERTY_KEY_C ((4 << 5) | 0x3)
#define QWERTY_KEY_V ((4 << 5) | 0x4)
#define QWERTY_KEY_B ((4 << 5) | 0x5)
#define QWERTY_KEY_N ((4 << 5) | 0x6)
#define QWERTY_KEY_M ((4 << 5) | 0x7)
#define QWERTY_KEY_COMMA ((4 << 5) | 0x8)
#define QWERTY_KEY_PERIOD ((4 << 5) | 0x9)
#define QWERTY_KEY_SLASH ((4 << 5) | 0xA)
#define QWERTY_KEY_R_SHIFT ((4 << 5) | 0xC)
#define QWERTY_KEY_L_CTRL (5 << 5)
#define QWERTY_KEY_WIN ((5 << 5) | 0x1)
#define QWERTY_KEY_L_ALT ((5 << 5) | 0x2)
#define QWERTY_KEY_SPACE ((5 << 5) | 0x3)
#define QWERTY_KEY_R_ALT ((5 << 5) | 0x8)
#define QWERTY_KEY_RN ((5 << 5) | 0xA)
#define QWERTY_KEY_MENU ((5 << 5) | 0xB)
#define QWERTY_KEY_R_CTRL ((5 << 5) | 0xC)

#define KEY_NONE 0xFF

typedef struct {
  uint8_t key_code; 
  /// If applicable
  uint8_t ascii_code;
  /// If not release
  bool key_press;
} key_event_t;

typedef void (*key_event_reciever)(key_event_t event);

typedef struct {
  bool (*check_keycode_down)(uint8_t keycode);
  bool (*register_key_event_handler)(key_event_reciever reciever);
  void (*unregister_key_event_handler)(key_event_reciever reciever);
  bool (*is_capslock_on)(void);
} hal_kbd_t;

#define KEY_EVENT_RECIEVER_COUNT 512

void hal_init_kbd(void);
hal_kbd_t hal_get_kbd(void);

#endif
