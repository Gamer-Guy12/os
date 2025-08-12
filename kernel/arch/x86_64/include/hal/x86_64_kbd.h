#ifndef X86_64_HAL_KBD
#define X86_64_HAL_KBD

#include <hal/kbd.h>
#include <stdbool.h>
#include <decls.h>

bool WUNUSED check_ps_2(void);

hal_kbd_t WUNUSED init_ps_2(void);

#endif

