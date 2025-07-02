#ifndef HAL_KBD_H
#define HAL_KBD_H

#include <stdbool.h>
#include <stdint.h>

typedef void (*ascii_key_down_handler)(uint8_t ascii);

typedef struct {
  bool (*check_ascii_down)(uint8_t key);
  void (*register_ascii_handler)(ascii_key_down_handler handler);
  void (*unregister_ascii_handler)(ascii_key_down_handler handler);
} hal_kbd_t;

void hal_init_kbd(void);
hal_kbd_t hal_get_kbd(void);

#endif
