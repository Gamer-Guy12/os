#include <hal/kbd.h>
#include <hal/hal.h>

void init_x86_64_hal(void) {
  hal_init_kbd();
}
