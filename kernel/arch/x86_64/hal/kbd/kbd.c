#include <cls.h>
#include <libk/err.h>
#include <libk/sys.h>
#include <hal/x86_64_kbd.h>
#include <hal/kbd.h>

void hal_init_kbd(void) {
  cls_t* cls = get_cls();

  if (!check_ps_2()) {
    cls->kbd_interface = init_ps_2();
    return;
  }   

  sys_panic(HAL_INIT_ERR | KBD_HANDLING_ERR);
}

