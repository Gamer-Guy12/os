#include <cls.h>
#include <hal/kbd.h>
#include <hal/x86_64_kbd.h>
#include <libk/err.h>
#include <libk/sys.h>

hal_kbd_t kbd;

void hal_init_kbd(void) {

  if (check_ps_2()) {
    kbd = init_ps_2();
    return;
  }

  sys_panic(HAL_INIT_ERR | KBD_HANDLING_ERR);
}

hal_kbd_t hal_get_kbd(void) {
  return kbd;
}

