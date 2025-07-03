#ifndef X86_64_HAL_KBD
#define X86_64_HAL_KBD

#include <hal/kbd.h>
#include <stdbool.h>

bool check_ps_2(void);

hal_kbd_t init_ps_2(void);

#endif

