#include <hal/kbd.h>
#include <ps_2.h>

bool check_ps_2(void) {
  // IMPORTANT: TODO: Do an actual check for the ps_2 controller

  return true;
}

void on_key_press(void) {
  handle_key_press();
}

hal_kbd_t init_ps_2(void) {
  init_ps_2_driver();

  hal_kbd_t kbd = { 0 };

  return kbd;
}

